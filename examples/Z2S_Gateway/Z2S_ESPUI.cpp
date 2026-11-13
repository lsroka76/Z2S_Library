#include "Z2S_ESPUI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

Z2S_ESPUIClass ESPUI;

namespace {

struct WsQueuedEvent {
  uint16_t id;
  int8_t cb_type;
  uint8_t is_custom;
};

static constexpr uint8_t WS_EV_QUEUE_SIZE = 32;
static WsQueuedEvent s_ev_queue[WS_EV_QUEUE_SIZE];
static volatile uint8_t s_ev_head = 0;
static volatile uint8_t s_ev_tail = 0;
static portMUX_TYPE s_ev_mux = portMUX_INITIALIZER_UNLOCKED;

#if Z2S_USE_PSRAM
EXT_RAM_BSS_ATTR static char s_batch_buf[4096];
#else
static char s_batch_buf[1360];
#endif

static size_t s_ui_stage_len = 0;
static size_t s_ui_stage_off = 0;
static size_t s_ui_ctrl_idx = 0;
static volatile bool s_ui_streaming = false;
static volatile uint32_t s_ui_stream_ms = 0;

static bool enqueueWsEvent(uint16_t id, int8_t cb_type, uint8_t is_custom) {
  bool queued = false;
  portENTER_CRITICAL(&s_ev_mux);
  uint8_t next_head = (s_ev_head + 1) % WS_EV_QUEUE_SIZE;
  if (next_head != s_ev_tail) {
    s_ev_queue[s_ev_head] = {id, cb_type, is_custom};
    s_ev_head = next_head;
    queued = true;
  }
  portEXIT_CRITICAL(&s_ev_mux);
  return queued;
}

static bool dequeueWsEvent(WsQueuedEvent &out) {
  bool has_ev = false;
  portENTER_CRITICAL(&s_ev_mux);
  if (s_ev_tail != s_ev_head) {
    out = s_ev_queue[s_ev_tail];
    s_ev_tail = (s_ev_tail + 1) % WS_EV_QUEUE_SIZE;
    has_ev = true;
  }
  portEXIT_CRITICAL(&s_ev_mux);
  return has_ev;
}

static void setControlStringLen(BasicControl *ctrl, const char *data, size_t len) {
  if (!ctrl) return;
  uint16_t needed = (len < 32) ? 32 : (len > 2048 ? 2048 : (uint16_t)len);
  if (ctrl->str_buf == nullptr || needed > ctrl->str_cap) {
    ctrl->reserve(needed);
  }
  if (ctrl->str_buf) {
    size_t copy_len = (len <= ctrl->str_cap) ? len : ctrl->str_cap;
    if (copy_len > 0 && data) {
      memcpy(ctrl->str_buf, data, copy_len);
    }
    ctrl->str_buf[copy_len] = '\0';
    ctrl->int_val = strtol(ctrl->str_buf, nullptr, 10);
  }
}

} // namespace

static bool appendEscapedJson(char *buf, size_t max_len, size_t &off, const char *str) {
  if (!str) return true;
  while (*str) {
    if (off + 6 >= max_len) return false;
    char c = *str++;
    if (c == '"' || c == '\\') { buf[off++] = '\\'; buf[off++] = c; }
    else if (c == '\n') { buf[off++] = '\\'; buf[off++] = 'n'; }
    else if (c == '\r') { buf[off++] = '\\'; buf[off++] = 'r'; }
    else if (c == '\t') { buf[off++] = '\\'; buf[off++] = 't'; }
    else if ((uint8_t)c >= 0x20) { buf[off++] = c; }
  }
  buf[off] = '\0';
  return true;
}

static bool appendFmt(char *buf, size_t max_len, size_t &off, const char *fmt, ...) {
  if (off >= max_len) return false;
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(buf + off, max_len - off, fmt, args);
  va_end(args);
  if (n < 0 || (size_t)n >= (max_len - off)) return false;
  off += n;
  return true;
}

static const char Z2S_INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Z2S Control Panel</title>
<style>
:root{--bg:#181b20;--card:#252a32;--card-hdr:#1f242b;--text:#f1f5f9;--muted:#cbd5e1;--accent:#3b82f6;--emerald:#10b981;--alizarin:#e74c3c;--border:#374151;--max-w:1020px}
*{box-sizing:border-box}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--text);font-size:13px;line-height:1.45}
header{background:#111317;border-bottom:1px solid var(--border);padding:10px 16px}
.hdr-inner{max-width:var(--max-w);margin:0 auto;display:flex;justify-content:space-between;align-items:center}
header h1{margin:0;font-size:16px;font-weight:700;letter-spacing:.5px;color:#fff}
#ws-status{font-size:11px;padding:3px 10px;border-radius:12px;font-weight:600;background:#dc2626;color:#fff}
#ws-status.online{background:#16a34a}
.tabs-bar{background:#14171c;border-bottom:1px solid var(--border);padding:10px 16px 0}
.tabs-nav{max-width:var(--max-w);margin:0 auto;display:flex;flex-wrap:wrap;gap:4px}
.tab-btn{background:#1e2229;color:#94a3b8;border:1px solid var(--border);border-bottom:none;padding:8px 13px;font-size:12px;font-weight:600;cursor:pointer;border-radius:6px 6px 0 0;transition:all .15s}
.tab-btn:hover{color:#fff;background:#282d37}
.tab-btn.active{color:#fff;background:var(--card);border-color:var(--accent);border-bottom:2px solid var(--card);margin-bottom:-1px}
.tab-pane{display:none;padding:14px 16px;max-width:var(--max-w);margin:0 auto}
.tab-pane.active{display:flex;flex-wrap:wrap;gap:12px;align-items:flex-start;justify-content:space-between}
.separator{width:100%;border-bottom:1px solid var(--border);padding:8px 4px 4px;margin-top:4px;font-weight:700;font-size:13px;color:#60a5fa;text-transform:uppercase;letter-spacing:.6px}
.panel{background:var(--card);border:1px solid var(--border);border-bottom:3px solid #111317;border-radius:6px;padding:12px 14px;width:calc(50% - 6px);display:flex;flex-wrap:wrap;justify-content:center;align-items:flex-end;align-content:flex-start;gap:0;box-shadow:0 3px 6px rgba(0,0,0,.25);text-align:center;transition:opacity .2s}
.panel.wide{width:100%}
.panel-col{width:calc(50% - 6px);display:flex;flex-direction:column;gap:12px}
.panel-col>.panel{width:100%}
.panel.panel-disabled{opacity:.45;background:#1c2026;border-color:#2d333b;border-bottom-color:#111317;pointer-events:none;filter:grayscale(30%)}
@media(max-width:760px){.panel,.panel-col{width:100%}}
.panel-hdr{width:100%;font-size:13px;font-weight:700;color:#fff;background:rgba(0,0,0,.22);padding:6px 10px;margin:-12px -14px 10px -14px;border-radius:5px 5px 0 0;border-bottom:1px solid var(--border);text-align:center}
.ctrl-wrap{display:inline-flex;flex-direction:column;justify-content:flex-end;align-items:center;width:100%;max-width:100%;margin:0 0 8px 0;vertical-align:bottom;transition:opacity .15s;min-width:0}
.panel:not(.panel-disabled) .ctrl-wrap.ctrl-disabled{opacity:.45;pointer-events:none}
.ctrl-wrap.ctrl-btn-inline{width:auto;flex:0 1 auto;margin:4px}
.ctrl-wrap.ctrl-btn-inline button.z2s-btn{width:auto;min-width:115px;padding:0 14px}
.ctrl-wrap.ctrl-sw-inline{width:205px;flex:0 1 205px;margin:4px 6px}
.ctrl-wrap.ctrl-sw-root{width:auto;flex:0 1 auto;margin:4px auto}
.ctrl-wrap.ctrl-line-break{width:100%!important;height:0!important;min-height:0!important;margin:2px 0!important;padding:0!important;overflow:hidden}
.ctrl-lbl{font-size:11.5px;color:var(--muted);margin-bottom:4px;line-height:1.3;text-align:center;width:100%;max-width:100%;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;cursor:pointer;padding:0 2px;user-select:none}
.ctrl-lbl:hover{color:#fff}
.ctrl-lbl.expanded{white-space:normal;overflow:visible;text-overflow:clip;word-break:break-word}
.ctrl-lbl:not(.expanded) .lbl-br{display:inline}
.ctrl-lbl.expanded .lbl-br{display:block;height:0}
.z2s-label{display:block;width:100%;padding:6px 10px;border-radius:4px;background:rgba(0,0,0,.22);color:var(--text);font-size:12.5px;word-break:break-word;text-align:center}
.z2s-label.z2s-label-clear{background:transparent!important;padding:2px 4px 6px!important;color:var(--muted);font-size:12px}
.z2s-label:empty{display:none}
input[type=text],input[type=password],input[type=number],select,textarea{background:#14171c!important;color:#fff!important;border:1px solid #4b5563!important;border-radius:4px!important;padding:6px 10px!important;font-size:12.5px!important;width:100%!important;height:34px;outline:none!important;margin:0!important}
textarea{height:auto!important;padding:8px 10px!important;font-family:monospace}
input:focus,select:focus,textarea:focus{border-color:var(--accent)!important;box-shadow:0 0 0 2px rgba(59,130,246,.25)!important}
input:disabled,select:disabled,textarea:disabled,button:disabled{opacity:.6;cursor:not-allowed}
button.z2s-btn{background:#3b4252;color:#fff;border:1px solid #4c566a;border-bottom:3px solid #2e3440;border-radius:4px;padding:0 12px;height:34px;width:100%;font-size:12px;font-weight:600;cursor:pointer;transition:all .12s;display:inline-flex;align-items:center;justify-content:center;white-space:nowrap}
button.z2s-btn:hover{background:#434c5e;border-color:#60a5fa}
button.z2s-btn:active{transform:translateY(1px)}
button.z2s-btn.alizarin{background:#c0392b;border-color:#e74c3c;border-bottom-color:#922b21}
button.z2s-btn.alizarin:hover{background:#e74c3c}
button.z2s-btn.emerald{background:#059669;border-color:#10b981;border-bottom-color:#065f46}
button.z2s-btn.emerald:hover{background:#10b981}
.sw-box{display:inline-flex;flex-direction:column;align-items:center;justify-content:center;background:transparent;border:none;padding:2px 4px;width:100%;gap:4px}
.switch{position:relative;display:inline-block;width:46px;height:24px;flex-shrink:0}
.switch input{opacity:0;width:0;height:0}
.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#4b5563;transition:.2s;border-radius:24px;border:1px solid #374151}
.slider:before{position:absolute;content:"";height:18px;width:18px;left:2px;bottom:2px;background:#fff;transition:.2s;border-radius:50%}
input:checked+.slider{background:var(--accent);border-color:#60a5fa}
input:checked+.slider.alizarin{background:var(--alizarin);border-color:#f87171}
input:checked+.slider:before{transform:translateX(22px)}
.z2s-force-hidden{display:none!important}
.subaction-helper-select:disabled{opacity:.5!important;pointer-events:none!important}
</style>
</head>
<body>
<header>
  <div class="hdr-inner">
    <h1 id="ui-title">ZIGBEE &lt;=&gt; SUPLA CONTROL PANEL</h1>
    <span id="ws-status">Connecting...</span>
  </div>
</header>
<div class="tabs-bar"><nav class="tabs-nav" id="tabs-nav"></nav></div>
<main id="tabs-content"></main>
<script>
window.websock=null;
window.__z2sUiReady=false;
window.z2sIrOptions=[];
window.z2sPoOptions=[];
window.z2sTableQueues={};
window.z2sActionEditActive=false;

function sendWs(msg){
  if(window.websock&&window.websock.readyState===WebSocket.OPEN)window.websock.send(msg);
}
function redirect2ZigbeeOTA(){window.location.assign("/zigbee_ota");}

async function downloadPartition(label,btn){
  if(btn)btn.innerText=`Downloading ${label}... 0%`;
  const res=await fetch(`/partition?label=${label}`);
  if(!res.ok)throw new Error(`Failed to fetch ${label} (HTTP ${res.status})`);
  const total=parseInt(res.headers.get("content-length")||"0",10);
  const reader=res.body.getReader();
  const chunks=[];
  let loaded=0;
  while(true){
    const {done,value}=await reader.read();
    if(done)break;
    chunks.push(value);
    loaded+=value.length;
    if(total&&btn)btn.innerText=`Downloading ${label}... ${Math.round((loaded/total)*100)}%`;
  }
  const url=window.URL.createObjectURL(new Blob(chunks,{type:"application/octet-stream"}));
  const a=document.createElement("a");
  a.style.display="none";
  a.href=url;
  a.download=`${window.location.hostname.replace(".local","")}_${label}_backup.bin`;
  document.body.appendChild(a);
  a.click();
  window.URL.revokeObjectURL(url);
  a.remove();
  return new Promise(r=>setTimeout(r,300));
}

async function backupAndRedirectToOTA(buttonId){
  const btn=document.getElementById(buttonId);
  const orig=btn?btn.innerText:"";
  if(btn)btn.disabled=true;
  try{
    await downloadPartition("nvs",btn);
    await downloadPartition("spiffs",btn);
    await downloadPartition("zb_storage",btn);
    if(btn)btn.innerText="Redirecting to OTA...";
    setTimeout(()=>window.location.assign("/update"),500);
  }catch(e){
    alert(`Backup failed: ${e.message}. Redirecting to OTA anyway...`);
    window.location.assign("/update");
    if(btn){btn.innerText=orig;btn.disabled=false;}
  }
}

async function backupAndContinue(buttonId){
  const btn=document.getElementById(buttonId);
  const orig=btn?btn.innerText:"";
  if(btn)btn.disabled=true;
  try{
    await downloadPartition("nvs",btn);
    await downloadPartition("spiffs",btn);
    await downloadPartition("zb_storage",btn);
  }finally{
    if(btn){btn.innerText=orig;btn.disabled=false;}
  }
  const id=buttonId.replace("btn","");
  sendWs("bdown:"+id);
  setTimeout(()=>sendWs("bup:"+id),50);
}

window.onGatewayCommand=function(cmd,labelId){
  if(cmd==="Z2S_GUI_CMD_START_BACKUP"){
    const el=document.getElementById("id"+labelId)||document.getElementById("l"+labelId);
    if(el)el.innerHTML="Downloading...";
    (async()=>{
      await downloadPartition("nvs",null);
      await downloadPartition("spiffs",null);
      await downloadPartition("zb_storage",null);
    })();
  }else if(cmd==="Z2S_GUI_CMD_UI_LOCKDOWN"){
    document.body.style.pointerEvents="none";
    document.body.style.opacity="0.5";
  }else if(cmd==="CMD_UI_UNLOCK"){
    document.body.style.pointerEvents="auto";
    document.body.style.opacity="1";
  }
};

window.buttonclick=function(id,isdown){
  const btn=document.getElementById("btn"+id);
  const txt=btn?btn.textContent.trim():"";
  if(id==30||txt==="Gateway firmware update"){if(!isdown)backupAndRedirectToOTA("btn"+id);return;}
  if(id==31||txt==="Upload Zigbee OTA file"){if(!isdown)redirect2ZigbeeOTA();return;}
  if(id==34||txt==="GitHub gateway firmware update"){if(!isdown)backupAndContinue("btn"+id);return;}
  sendWs((isdown?"bdown:":"bup:")+id);
};

window.numberchange=function(id){
  const el=document.getElementById("num"+id)||document.getElementById("text"+id);
  if(el)sendWs("nvalue:"+id+":"+el.value);
};

function activateTab(tabId){
  document.querySelectorAll(".tab-btn").forEach(b=>b.classList.toggle("active",b.dataset.tab==tabId));
  document.querySelectorAll(".tab-pane").forEach(p=>p.classList.toggle("active",p.id==="tab"+tabId));
  sendWs("tabvalue:"+tabId);
}

function setCtrlLabelHtml(lblEl,rawHtml){
  if(!lblEl)return;
  lblEl.innerHTML=String(rawHtml??"").replace(/<br\s*\/?>/gi,'<span class="lbl-br"> </span>');
  lblEl.title=(lblEl.textContent||"").trim();
}

function syncLabelVisibility(wrap,el){
  if(!wrap||!el||Number(wrap.dataset.ctrlType)!==2)return;
  const isEmpty=!el.innerHTML||el.innerHTML.trim()==="";
  const isRoot=wrap.dataset.isRoot==="1";
  const isClear=el.classList.contains("z2s-label-clear");
  if(isEmpty&&isRoot){
    wrap.style.display="none";
  }else if(isEmpty&&isClear){
    wrap.classList.add("ctrl-line-break");
    wrap.style.display="";
  }else{
    wrap.classList.remove("ctrl-line-break");
    if(wrap.dataset.psHidden!=="1")wrap.style.display="";
  }
}

function applyCleanStyles(wrap,el,esStr,psStr,enabled,isRootPanel,ctrlType,parentPanel){
  const targetPanel=parentPanel||(isRootPanel?wrap.parentElement:null);
  const canDisableWholePanel=isRootPanel&&ctrlType===2;

  if(enabled!==undefined){
    wrap.classList.toggle("ctrl-disabled",!enabled);
    if(el)el.disabled=!enabled;
    if(canDisableWholePanel&&targetPanel)targetPanel.classList.toggle("panel-disabled",!enabled);
  }
  if(psStr!==undefined&&psStr!==null){
    if(psStr.includes("display: none")){
      wrap.dataset.psHidden="1";
      wrap.style.display="none";
      if(isRootPanel&&targetPanel)targetPanel.style.display="none";
    }else if(psStr.includes("#bbb")){
      wrap.dataset.psHidden="0";
      wrap.classList.add("ctrl-disabled");
      if(canDisableWholePanel&&targetPanel)targetPanel.classList.add("panel-disabled");
    }else if(psStr.length>0){
      wrap.dataset.psHidden="0";
      if(el&&!el.disabled)wrap.classList.remove("ctrl-disabled");
      if(canDisableWholePanel&&targetPanel&&(!el||!el.disabled))targetPanel.classList.remove("panel-disabled");
    }
  }
  if(esStr!==undefined&&esStr!==null){
    const hasW=esStr.includes("width:");
    if(esStr.length>0){
      if(ctrlType===2){
        if(esStr.includes("background-color: unset")||esStr.includes("background-color: transparent")){
          if(el)el.classList.add("z2s-label-clear");
        }
        if(el){
          el.style.cssText=esStr;
          el.style.width="100%";
          el.style.minWidth="0";
          el.style.margin="0";
          el.style.opacity="1";
        }
        wrap.style.cssText+=";"+esStr;
      }else{
        wrap.style.cssText+=";"+esStr;
        if(el&&esStr.includes("resize:"))el.style.resize="none";
      }
    }
    if(ctrlType===3){
      wrap.classList.toggle("ctrl-btn-inline",!hasW);
    }else if(ctrlType===4){
      if(isRootPanel&&!hasW)wrap.classList.add("ctrl-sw-root");
      else wrap.classList.toggle("ctrl-sw-inline",!hasW);
    }
  }
  if(ctrlType===2)syncLabelVisibility(wrap,el);
}

function createControlDom(c,isRootPanel,parentPanel){
  const wrap=document.createElement("div");
  wrap.className="ctrl-wrap";
  wrap.id="wrap"+c.id;
  wrap.dataset.isRoot=isRootPanel?"1":"0";
  wrap.dataset.ctrlType=c.t;

  if(!isRootPanel&&c.l&&c.t!==3&&c.t!==4){
    const lbl=document.createElement("div");
    lbl.className="ctrl-lbl";
    lbl.id="lbl"+c.id;
    setCtrlLabelHtml(lbl,c.l);
    lbl.onclick=()=>lbl.classList.toggle("expanded");
    wrap.appendChild(lbl);
  }
  let el=null;
  if(c.t===2){
    el=document.createElement("div");
    el.className="z2s-label";
    el.id="id"+c.id;
    el.innerHTML=c.v??"";
    const lAlias=document.createElement("span");
    lAlias.id="l"+c.id;
    lAlias.style.display="none";
    wrap.appendChild(lAlias);
  }else if(c.t===3){
    el=document.createElement("button");
    el.type="button";
    el.className="z2s-btn"+(c.c===6?" alizarin":(c.c===1?" emerald":""));
    el.id="btn"+c.id;
    el.innerHTML=c.v||c.l||"Button";
    el.onmousedown=()=>window.buttonclick(c.id,true);
    el.onmouseup=()=>window.buttonclick(c.id,false);
  }else if(c.t===4){
    const swBox=document.createElement("div");
    swBox.className="sw-box";
    if(!isRootPanel&&c.l){
      const st=document.createElement("div");
      st.className="ctrl-lbl";
      st.style.marginBottom="2px";
      st.id="lbl"+c.id;
      setCtrlLabelHtml(st,c.l);
      st.onclick=(e)=>{e.stopPropagation();st.classList.toggle("expanded");};
      swBox.appendChild(st);
    }
    const sw=document.createElement("label");
    sw.className="switch";
    el=document.createElement("input");
    el.type="checkbox";
    el.id="s"+c.id;
    el.checked=Number(c.v)>0;
    el.onchange=()=>sendWs((el.checked?"sactive:":"sinactive:")+c.id);
    const sp=document.createElement("span");
    sp.className="slider"+(c.c===6?" alizarin":"");
    sw.appendChild(el);
    sw.appendChild(sp);
    swBox.appendChild(sw);
    wrap.appendChild(swBox);
  }else if(c.t===5){
    el=document.createElement("input");
    el.type="number";
    el.id="num"+c.id;
    el.value=c.v??0;
    el.onchange=()=>window.numberchange(c.id);
  }else if(c.t===6){
    if(c.f&1){
      el=document.createElement("textarea");
      el.rows=c.r||3;
      el.cols=c.k||48;
    }else{
      el=document.createElement("input");
      el.type=(c.f&2)?"password":"text";
    }
    el.id="text"+c.id;
    el.value=c.v??"";
    el.onchange=()=>sendWs("tvalue:"+c.id+":"+el.value);
  }else if(c.t===7){
    el=document.createElement("select");
    el.id="select"+c.id;
    el.dataset.initVal=String(c.v??-1);
    el.onchange=()=>{
      el.dataset.initVal=String(el.value);
      updateActionThresholdsVisibility();
      syncSubactionDropdown();
      sendWs("slvalue:"+c.id+":"+el.value);
    };
  }
  if(el&&c.t!==4)wrap.appendChild(el);
  applyCleanStyles(wrap,el,c.es||"",c.ps||"",Boolean(c.e),isRootPanel,c.t,parentPanel);
  return wrap;
}

function updateSelectOptions(targetSelects,optionsData,selectedValue=null,clearExisting=true){
  const targets=Array.isArray(targetSelects)?targetSelects:[targetSelects];
  let optionsList=[];
  if(Array.isArray(optionsData)){
    optionsList=optionsData.map(item=>Array.isArray(item)
      ?{value:item[0],label:item[1],selected:Boolean(item[2])}
      :{value:item.value??item.val??item.id,label:item.label??item.name??item.text,selected:Boolean(item.selected??item.isSelected)}
    );
  }else if(typeof optionsData==="object"&&optionsData!==null){
    optionsList=Object.entries(optionsData).map(([value,label])=>({value,label,selected:false}));
  }
  const targetSelectedStr=(selectedValue!==null&&selectedValue!==undefined)?String(selectedValue):null;
  targets.forEach(target=>{
    const selectEl=(typeof target==="string")
      ?(document.getElementById(target)||document.querySelector(`select[name="${target}"]`)||document.querySelector(target))
      :(target instanceof HTMLElement?target:null);
    if(!selectEl)return;
    if(clearExisting){
      selectEl.innerHTML="";
      if(targetSelectedStr!==null)selectEl.dataset.initVal=targetSelectedStr;
    }
    const desiredVal=targetSelectedStr!==null?targetSelectedStr:(selectEl.dataset.initVal??null);
    const fragment=document.createDocumentFragment();
    optionsList.forEach(({value,label,selected})=>{
      const opt=document.createElement("option");
      opt.value=value;
      opt.textContent=label;
      if(desiredVal!==null&&String(value)===desiredVal)opt.selected=true;
      else if(selected)opt.selected=true;
      fragment.appendChild(opt);
    });
    selectEl.appendChild(fragment);
    if(desiredVal!==null&&Array.from(selectEl.options).some(o=>String(o.value)===desiredVal)){
      selectEl.value=desiredVal;
    }
  });
  updateActionThresholdsVisibility();
  syncSubactionDropdown();
}

function sendActionClickToESP32(actionIndex){
  const hiddenInput=document.getElementById("num2");
  if(hiddenInput){hiddenInput.value=actionIndex;window.numberchange(2);}
}

function formatActionTableCell(cellContent,colIdx,isIr,isPo){
  const str=String(cellContent);
  if(isIr||isPo)return str;
  if(colIdx===0){
    const lower=str.toLowerCase();
    if(lower.endsWith(" (on)"))return `<span class="action-name-cell"><span class="action-btn-on">ON</span><span class="action-name-text">${str.slice(0,-5)}</span></span>`;
    if(lower.endsWith(" (off)"))return `<span class="action-name-cell"><span class="action-btn-off">OFF</span><span class="action-name-text">${str.slice(0,-6)}</span></span>`;
    return `<span class="action-name-cell"><span class="action-name-text">${str}</span></span>`;
  }
  if(colIdx===1||colIdx===3){
    let metaIdx=str.indexOf(" (v:");
    if(metaIdx===-1)metaIdx=str.indexOf(" (sid:");
    if(metaIdx!==-1&&str.endsWith(")"))return `<span class="action-flat-btn">${str.slice(0,metaIdx)}</span><span class="action-btn-meta">${str.slice(metaIdx)}</span>`;
    return `<span class="action-flat-btn">${str}</span>`;
  }
  if(colIdx===2||colIdx===4){
    const safe=str.split('"').join("&quot;");
    return `<span class="channel-cell-wrap"><span class="channel-chain-btn" data-channel="${safe}" title="Filter actions linked to this channel">&#128279;</span><span class="channel-cell-name">${str}</span></span>`;
  }
  return str;
}

function filterActionTable(targetId){
  const targetLabel=document.getElementById(targetId);
  if(!targetLabel)return;
  const searchInput=targetLabel.querySelector(".table-search-input");
  const clearBtn=targetLabel.querySelector(".table-search-clear");
  const countBadge=targetLabel.querySelector(".table-search-count");
  const tbody=targetLabel.querySelector("tbody");
  if(!searchInput||!tbody)return;
  const query=searchInput.value.trim().toLowerCase();
  targetLabel.dataset.searchQuery=searchInput.value;
  if(clearBtn)clearBtn.style.display=query.length>0?"inline-flex":"none";
  const isIr=targetLabel.dataset.isIr==="1";
  const isPo=targetLabel.dataset.isPo==="1";
  const colSpan=(isIr||isPo)?3:5;
  const searchCols=(isIr||isPo)?[1]:[0,2,4];
  const rows=tbody.querySelectorAll("tr[data-action-idx]");
  let visibleCount=0;
  rows.forEach(row=>{
    const cells=row.querySelectorAll("td");
    let rowMatches=!query;
    searchCols.forEach(colIdx=>{
      const cell=cells[colIdx];
      if(!cell)return;
      let rawText=cell.getAttribute("title")||cell.textContent||"";
      if(!isIr&&!isPo&&colIdx===0){
        const lr=rawText.toLowerCase();
        if(lr.endsWith(" (on)"))rawText=rawText.slice(0,-5);
        else if(lr.endsWith(" (off)"))rawText=rawText.slice(0,-6);
      }
      const colHit=Boolean(query)&&rawText.toLowerCase().includes(query);
      if(colHit)rowMatches=true;
      if(!isIr&&!isPo&&(colIdx===2||colIdx===4)){
        const wrap=cell.querySelector(".channel-cell-wrap");
        if(wrap){
          wrap.classList.toggle("chain-match-src",colHit&&colIdx===2);
          wrap.classList.toggle("chain-match-dst",colHit&&colIdx===4);
        }
      }
    });
    row.style.display=rowMatches?"":"none";
    if(rowMatches)visibleCount++;
  });
  let noMatchRow=tbody.querySelector("tr.no-search-match");
  if(rows.length>0&&visibleCount===0){
    if(!noMatchRow){
      noMatchRow=document.createElement("tr");
      noMatchRow.className="no-search-match";
      noMatchRow.innerHTML=`<td colspan="${colSpan}" style="text-align:center;padding:16px;color:#94a3b8;font-style:italic;">No matching items found</td>`;
      tbody.appendChild(noMatchRow);
    }
    noMatchRow.style.display="";
  }else if(noMatchRow){
    noMatchRow.style.display="none";
  }
  if(countBadge)countBadge.textContent=query?`${visibleCount} / ${rows.length}`:`${rows.length}`;
}

function updateActionTable(data){
  if(!data||!data.target)return;
  const targetLabel=document.getElementById(data.target);
  if(targetLabel)renderActionTableImmediate(targetLabel,data);
}

function renderActionTableImmediate(targetLabel,data){
  const isIr=Boolean(data.is_ir);
  const isPo=Boolean(data.is_po);
  const colSpan=(isIr||isPo)?3:5;
  const emptyMsg=isIr?"No IR codes saved for this device":(isPo?"No Pushover messages available":"No actions available");
  const searchPlaceholder=isIr?"Search IR command by name...":(isPo?"Search Pushover message by name...":"Search by action, source, or destination channel...");
  targetLabel.dataset.isIr=isIr?"1":"0";
  targetLabel.dataset.isPo=isPo?"1":"0";
  const savedSearch=targetLabel.dataset.searchQuery||"";
  const isExpanded=targetLabel.dataset.isExpanded==="1";
  let rowsHtml="";
  if(!data.d||data.d.length===0){
    rowsHtml=`<tr><td colspan="${colSpan}" style="text-align:center;padding:16px;color:#94a3b8;font-style:italic;">${emptyMsg}</td></tr>`;
  }else{
    if(!data.append){
      if(isIr)window.z2sIrOptions=[];
      if(isPo)window.z2sPoOptions=[];
    }
    data.d.forEach((row,index)=>{
      if((isIr||isPo)&&row.length>=2){
        const slotId=parseInt(String(row[0]).replace("#",""),10);
        if(!isNaN(slotId)){
          const entry={id:slotId,label:`${row[0]} - ${row[1]}`};
          if(isIr)window.z2sIrOptions.push(entry);
          if(isPo)window.z2sPoOptions.push(entry);
        }
      }
      const absoluteIndex=(data.start_idx!==undefined?data.start_idx:0)+index;
      const style=(absoluteIndex===data.s)?"background-color:rgba(59,130,246,0.28);font-weight:bold;":"font-weight:normal;";
      rowsHtml+=`<tr style="${style}" data-action-idx="${absoluteIndex}">`;
      row.forEach((cell,colIdx)=>{
        const cleanCell=String(cell).split('"').join("&quot;");
        rowsHtml+=`<td style="padding:4px 6px;border-bottom:1px solid rgba(255,255,255,0.08);vertical-align:middle;" title="${cleanCell}">${formatActionTableCell(cell,colIdx,isIr,isPo)}</td>`;
      });
      rowsHtml+="</tr>";
    });
  }
  if(data.append){
    const tbody=targetLabel.querySelector("tbody");
    if(tbody){tbody.insertAdjacentHTML("beforeend",rowsHtml);filterActionTable(data.target);}
    return;
  }
  const T=`#${data.target}`;
  let html=`<style>`+
    `${T} tbody tr[data-action-idx]{cursor:pointer;transition:background .15s}`+
    `${T} tbody tr[data-action-idx]:hover{background-color:rgba(59,130,246,.22)!important}`+
    `${T} th{position:sticky;top:0;background:#1e2229;color:#e2e8f0;font-weight:600;border-bottom:2px solid #475569;z-index:10}`+
    `${T} th,${T} td{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}`+
    `${T} td{font-size:11px}`+
    `${T} .table-search-bar{display:flex;align-items:center;gap:8px;margin-top:4px;padding:5px 10px;background:rgba(0,0,0,.25);border:1px solid rgba(128,128,128,.25);border-bottom:none;border-radius:6px 6px 0 0}`+
    `${T} .table-search-input{flex:1;background:rgba(255,255,255,.08)!important;border:1px solid rgba(255,255,255,.2)!important;border-radius:4px!important;color:#f8fafc!important;padding:4px 8px!important;font-size:12px!important;outline:none!important;margin:0!important;height:26px!important}`+
    `${T} .table-search-clear,${T} .table-expand-btn{display:inline-flex;align-items:center;justify-content:center;background:rgba(255,255,255,.12);border:1px solid rgba(255,255,255,.2);color:#cbd5e1;border-radius:4px;height:24px;font-size:11px;font-weight:600;cursor:pointer;user-select:none;flex-shrink:0}`+
    `${T} .table-search-clear{display:none;padding:0 7px}`+
    `${T} .table-expand-btn{width:24px;font-size:12px}`+
    `${T} .table-search-count{font-size:11px;font-weight:600;color:#94a3b8;min-width:36px;text-align:right}`+
    `${T} .action-name-cell{display:inline-flex;align-items:center;gap:6px;font-size:11px;max-width:100%}`+
    `${T} .action-name-text,${T} .channel-cell-name{font-size:11px;overflow:hidden;text-overflow:ellipsis}`+
    `${T} .action-btn-on,${T} .action-btn-off{display:inline-flex;align-items:center;justify-content:center;padding:2px 6px;font-size:10px;font-weight:700;border-radius:4px;flex-shrink:0}`+
    `${T} .action-btn-on{background:#2ecc71;color:#fff;border:1px solid #27ae60}`+
    `${T} .action-btn-off{background:#e74c3c;color:#fff;border:1px solid #c0392b}`+
    `${T} .action-flat-btn{display:inline-block;padding:1px 6px;font-size:11px;font-weight:600;border-radius:4px;background:rgba(255,255,255,.08);border:1px solid rgba(255,255,255,.18);color:inherit}`+
    `${T} .action-btn-meta{font-size:11px;opacity:.8;margin-left:3px}`+
    `${T} .channel-cell-wrap{display:inline-flex;align-items:center;gap:5px;padding:1px 5px;border-radius:4px;max-width:100%}`+
    `${T} .channel-chain-btn{display:inline-flex;align-items:center;justify-content:center;font-size:10px;padding:1px 4px;border-radius:3px;background:rgba(255,255,255,.1);border:1px solid rgba(255,255,255,.2);cursor:pointer;opacity:.75;flex-shrink:0}`+
    `${T} .chain-match-src{background:rgba(59,130,246,.22);border:1px solid rgba(96,165,250,.55);color:#93c5fd;font-weight:600}`+
    `${T} .chain-match-dst{background:rgba(245,158,11,.22);border:1px solid rgba(251,191,36,.55);color:#fcd34d;font-weight:600}`+
    `${T} div[tabindex]{height:210px;overflow:auto;overscroll-behavior:contain;border:1px solid rgba(128,128,128,.25);transition:height .25s ease}`+
    `${T} div[tabindex].table-fullscreen{height:70vh!important}`+
    `${T} .table-footer-hints{display:flex;flex-wrap:wrap;gap:14px;justify-content:center;align-items:center;background:rgba(0,0,0,.3);border:1px solid rgba(255,255,255,.15);border-top:none;border-radius:0 0 6px 6px;padding:5px 10px;font-size:10px;color:#cbd5e1}`+
    `${T} .table-footer-hints kbd{background:#334155;border:1px solid #475569;border-radius:3px;color:#f8fafc;font-size:9px;font-weight:600;padding:2px 4px}`+
    `${T} .table-disabled{pointer-events:none!important;overflow-y:hidden!important;opacity:.5!important;filter:grayscale(50%)}`+
    `${T} .table-status-badge{display:inline-block;padding:2px 8px;border-radius:12px;font-size:.7rem;font-weight:600;text-transform:uppercase}`+
    `${T} .badge-active{background:#065f46;color:#d1fae5;border:1px solid #10b981}`+
    `${T} .badge-locked{background:#7f1d1d;color:#fee2e2;border:1px solid #ef4444}`+
  `</style>`;
  const headerHtml=isIr
    ?"<th style='padding:5px 6px;width:80px;'>ID (sid)</th><th style='padding:5px 6px;width:220px;'>Command Name</th><th style='padding:5px 6px;width:420px;'>IR Code (Base64)</th>"
    :isPo
    ?"<th style='padding:5px 6px;width:80px;'>ID (sid)</th><th style='padding:5px 6px;width:220px;'>Message Name</th><th style='padding:5px 6px;width:420px;'>Message Text</th>"
    :"<th style='padding:5px 6px;width:135px;'>Name (state)</th><th style='padding:5px 6px;width:135px;'>Supla event</th><th style='padding:5px 6px;width:180px;'>Source channel</th><th style='padding:5px 6px;width:120px;'>Supla action</th><th style='padding:5px 6px;width:180px;'>Destination channel</th>";
  const safeSavedSearch=savedSearch.split('"').join("&quot;");
  html+=`<div class="table-search-bar">`+
    `<input type="text" class="table-search-input" placeholder="\uD83D\uDD0D ${searchPlaceholder}" value="${safeSavedSearch}" autocomplete="off">`+
    `<span class="table-search-clear" title="Clear search">&#10005;</span>`+
    `<span class="table-search-count">0</span>`+
    `<span class="table-expand-btn" title="Toggle table height">${isExpanded?"&#10529;":"&#10530;"}</span>`+
  `</div>`+
  `<div tabindex="0" class="${isExpanded?"table-fullscreen":""}">`+
    `<table style="width:100%;min-width:750px;table-layout:fixed;border-collapse:collapse;text-align:left;">`+
      `<thead><tr>${headerHtml}</tr></thead><tbody>${rowsHtml}</tbody>`+
    `</table>`+
  `</div>`+
  `<div class="table-footer-hints">`+
    `<span><span class="table-status-badge badge-active">Active</span></span>`+
    `<span><kbd>&uarr;</kbd><kbd>&darr;</kbd> Navigate</span>`+
    `<span><kbd>PgUp</kbd><kbd>PgDn</kbd> &plusmn;5 Rows</span>`+
    `<span><kbd>Home</kbd><kbd>End</kbd> First/Last</span>`+
  `</div>`;
  targetLabel.innerHTML=html;
  targetLabel.style.display="block";
  if(targetLabel.parentElement){
    targetLabel.parentElement.classList.remove("ctrl-line-break");
    targetLabel.parentElement.style.display="";
  }
  const searchInput=targetLabel.querySelector(".table-search-input");
  const clearBtn=targetLabel.querySelector(".table-search-clear");
  const expandBtn=targetLabel.querySelector(".table-expand-btn");
  const container=targetLabel.querySelector("div[tabindex='0']");
  const tbody=targetLabel.querySelector("tbody");
  if(searchInput){
    searchInput.addEventListener("input",()=>filterActionTable(data.target));
    searchInput.addEventListener("keydown",(e)=>{
      if(e.key==="Escape"){searchInput.value="";filterActionTable(data.target);}
      else if(e.key==="ArrowDown"&&container){e.preventDefault();container.focus();}
    });
  }
  if(clearBtn&&searchInput){
    clearBtn.addEventListener("click",()=>{searchInput.value="";filterActionTable(data.target);searchInput.focus();});
  }
  if(expandBtn&&container){
    expandBtn.addEventListener("click",()=>{
      const isFull=container.classList.toggle("table-fullscreen");
      targetLabel.dataset.isExpanded=isFull?"1":"0";
      expandBtn.innerHTML=isFull?"&#10529;":"&#10530;";
    });
  }
  filterActionTable(data.target);
  if(tbody&&container){
    const activateRow=(rowElement,shouldScroll=true)=>{
      if(!rowElement)return;
      const actionIndex=parseInt(rowElement.getAttribute("data-action-idx"),10);
      if(isNaN(actionIndex))return;
      highlightActionRow({target:data.target,s:actionIndex},shouldScroll);
      const clickMask=isIr?0x4000:(isPo?0x2000:0);
      sendActionClickToESP32(actionIndex|clickMask);
    };
    tbody.addEventListener("click",function(e){
      const chainBtn=e.target.closest(".channel-chain-btn");
      if(chainBtn&&searchInput){
        e.stopPropagation();
        const chName=chainBtn.getAttribute("data-channel")||"";
        searchInput.value=(searchInput.value.trim().toLowerCase()===chName.trim().toLowerCase())?"":chName;
        filterActionTable(data.target);
        return;
      }
      const clickedRow=e.target.closest("tr[data-action-idx]");
      if(!clickedRow)return;
      container.focus();
      activateRow(clickedRow,false);
    });
    container.addEventListener("keydown",function(e){
      const navKeys=["ArrowUp","ArrowDown","Home","End","PageUp","PageDown","Enter"," "];
      if(!navKeys.includes(e.key))return;
      const visibleRows=Array.from(tbody.querySelectorAll("tr[data-action-idx]")).filter(r=>r.style.display!=="none");
      if(!visibleRows.length)return;
      let currentIndex=visibleRows.findIndex(r=>r.style.fontWeight==="bold");
      if(currentIndex===-1)currentIndex=0;
      if(e.key==="Enter"||e.key===" "){e.preventDefault();visibleRows[currentIndex].click();return;}
      let nextIndex=currentIndex;
      if(e.key==="ArrowUp")nextIndex=Math.max(0,currentIndex-1);
      else if(e.key==="ArrowDown")nextIndex=Math.min(visibleRows.length-1,currentIndex+1);
      else if(e.key==="PageUp")nextIndex=Math.max(0,currentIndex-5);
      else if(e.key==="PageDown")nextIndex=Math.min(visibleRows.length-1,currentIndex+5);
      else if(e.key==="Home")nextIndex=0;
      else if(e.key==="End")nextIndex=visibleRows.length-1;
      if(nextIndex!==currentIndex){e.preventDefault();activateRow(visibleRows[nextIndex],true);}
    });
  }
}

window.setTableDisabled=function(targetId,isDisabled){
  const targetLabel=document.getElementById(targetId);
  if(!targetLabel)return;
  if(targetLabel.dataset.isIr!=="1"&&targetLabel.dataset.isPo!=="1")window.z2sActionEditActive=Boolean(isDisabled);
  const container=targetLabel.querySelector("div[tabindex]");
  const searchBar=targetLabel.querySelector(".table-search-bar");
  const searchInput=targetLabel.querySelector(".table-search-input");
  const badge=targetLabel.querySelector(".table-status-badge");
  if(isDisabled){
    if(container){container.classList.add("table-disabled");container.setAttribute("tabindex","-1");container.blur();}
    if(searchBar)searchBar.classList.add("table-disabled");
    if(searchInput)searchInput.disabled=true;
    if(badge){badge.textContent="Locked";badge.className="table-status-badge badge-locked";}
  }else{
    if(container){container.classList.remove("table-disabled");container.setAttribute("tabindex","0");}
    if(searchBar)searchBar.classList.remove("table-disabled");
    if(searchInput)searchInput.disabled=false;
    if(badge){badge.textContent="Active";badge.className="table-status-badge badge-active";}
  }
  setTimeout(()=>{updateActionThresholdsVisibility();syncSubactionDropdown();},50);
};

function highlightActionRow(data,shouldScroll=true){
  const targetLabel=document.getElementById(data.target);
  if(!targetLabel)return;
  targetLabel.querySelectorAll("tbody tr[data-action-idx]").forEach(row=>{
    const rowIdx=parseInt(row.getAttribute("data-action-idx"),10);
    if(rowIdx===data.s){
      row.style.backgroundColor="rgba(59, 130, 246, 0.28)";
      row.style.fontWeight="bold";
      if(shouldScroll&&row.style.display!=="none")row.scrollIntoView({behavior:"smooth",block:"nearest"});
    }else{
      row.style.backgroundColor="";
      row.style.fontWeight="normal";
    }
  });
}

function editActionRow(data){
  const targetLabel=document.getElementById(data.target);
  if(!targetLabel)return;
  const isIr=targetLabel.dataset.isIr==="1";
  const isPo=targetLabel.dataset.isPo==="1";
  const targetRow=targetLabel.querySelector(`tbody tr[data-action-idx="${data.idx}"]`)||
                  targetLabel.querySelectorAll("tbody tr[data-action-idx]")[data.idx];
  if(targetRow&&Array.isArray(data.d)){
    const cells=targetRow.querySelectorAll("td");
    data.d.forEach((newCellContent,i)=>{
      if(cells[i]){
        cells[i].innerHTML=formatActionTableCell(newCellContent,i,isIr,isPo);
        cells[i].title=String(newCellContent);
      }
    });
    if((isIr||isPo)&&data.d.length>=2){
      const slotId=parseInt(String(data.d[0]).replace("#",""),10);
      const newLabel=`${data.d[0]} - ${data.d[1]}`;
      const targetList=isIr?window.z2sIrOptions:window.z2sPoOptions;
      if(targetList&&!isNaN(slotId)){
        const item=targetList.find(opt=>opt.id===slotId);
        if(item)item.label=newLabel;
        else targetList.push({id:slotId,label:newLabel});
        syncSubactionDropdown();
      }
    }
    filterActionTable(data.target);
  }
}

function updateActionThresholdsVisibility(){
  let eventSelect=null;
  for(const sel of document.querySelectorAll("select")){
    if(sel.options.length>0&&sel.options[0].text.includes("Select source event")){eventSelect=sel;break;}
  }
  if(!eventSelect)return;
  const panel=eventSelect.closest(".panel")||eventSelect.parentElement.parentElement;
  if(!panel)return;
  let thresh1Wrap=null,thresh2Wrap=null;
  panel.querySelectorAll("input[type='text']").forEach(inp=>{
    const wrap=inp.parentElement;
    if(!wrap)return;
    const lt=wrap.textContent.toLowerCase();
    if(lt.includes("main threshold"))thresh1Wrap=wrap;
    else if(lt.includes("secondary threshold"))thresh2Wrap=wrap;
  });
  if(!thresh1Wrap||!thresh2Wrap)return;
  const val=parseInt(eventSelect.value,10);
  const selectedText=eventSelect.options[eventSelect.selectedIndex]?eventSelect.options[eventSelect.selectedIndex].text.toLowerCase():"";
  const isCondition=!isNaN(val)&&val>=0xFFFF;
  thresh1Wrap.style.display=isCondition?"":"none";
  thresh2Wrap.style.display=(isCondition&&selectedText.includes("between"))?"":"none";
}

function syncSubactionDropdown(){
  let subInput=null;
  document.querySelectorAll("input").forEach(inp=>{
    const wrap=inp.parentElement;
    if(wrap&&wrap.textContent.includes("Subaction ID"))subInput=inp;
  });
  if(!subInput)return;
  let actionSelect=null;
  for(const sel of document.querySelectorAll("select")){
    if(sel.options.length>0&&sel.options[0].text.includes("Select destination action")){actionSelect=sel;break;}
  }
  if(!actionSelect)return;
  const isFormDisabled=!window.z2sActionEditActive||subInput.disabled||actionSelect.disabled;
  const selectedActionText=(!isFormDisabled&&actionSelect.selectedIndex>=0&&actionSelect.options[actionSelect.selectedIndex])
    ?actionSelect.options[actionSelect.selectedIndex].text.toUpperCase():"";
  const isIrMode=selectedActionText.includes("SEND IR CODE");
  const isPoMode=selectedActionText.includes("SEND PUSHOVER MESSAGE");
  let helperSelect=subInput.parentElement.querySelector(".subaction-helper-select");
  if(!helperSelect){
    helperSelect=document.createElement("select");
    helperSelect.className="subaction-helper-select z2s-force-hidden";
    subInput.insertAdjacentElement("afterend",helperSelect);
    helperSelect.addEventListener("change",function(){
      subInput.value=this.value;
      const numId=subInput.id.replace("num","").replace("text","");
      if(numId)window.numberchange(numId);
    });
  }
  if(isFormDisabled||(!isIrMode&&!isPoMode)){
    helperSelect.value="0";
    helperSelect.disabled=true;
    helperSelect.classList.add("z2s-force-hidden");
    subInput.classList.remove("z2s-force-hidden");
    return;
  }
  const optionsData=isIrMode?(window.z2sIrOptions||[]):(window.z2sPoOptions||[]);
  const placeholder=isIrMode?"Select IR code (sid)...":"Select Pushover message (sid)...";
  const currentVal=String(subInput.value||"0");
  let html=`<option value="0">${placeholder}</option>`;
  if(optionsData.length===0){
    html+=`<option value="${currentVal}" selected>No saved items (sid: ${currentVal})</option>`;
  }else{
    optionsData.forEach(opt=>{
      const selected=String(opt.id)===currentVal?" selected":"";
      html+=`<option value="${opt.id}"${selected}>${opt.label.split('"').join("&quot;")}</option>`;
    });
  }
  helperSelect.innerHTML=html;
  helperSelect.disabled=false;
  subInput.classList.add("z2s-force-hidden");
  helperSelect.classList.remove("z2s-force-hidden");
}

function applySingleControlUpdate(u){
  const id=u.id;
  if(id===undefined||id<0)return;
  const wrap=document.getElementById("wrap"+id);
  const el=document.getElementById("id"+id)||
           document.getElementById("text"+id)||
           document.getElementById("num"+id)||
           document.getElementById("select"+id)||
           document.getElementById("s"+id)||
           document.getElementById("btn"+id);
  const val=(u.v!==undefined)?u.v:(u.op==="val"?u.value:undefined);
  if(val!==undefined&&el){
    if(el.type==="checkbox")el.checked=Number(val)>0;
    else if(el.tagName==="DIV"||el.tagName==="BUTTON"){
      el.innerHTML=val;
      if(el.tagName==="DIV")syncLabelVisibility(wrap,el);
    }else if(el.tagName==="SELECT"){
      el.dataset.initVal=String(val);
      el.value=String(val);
    }else el.value=val;
  }
  const en=(u.e!==undefined)?Boolean(u.e):(u.op==="en"?Boolean(u.enabled):undefined);
  if(en!==undefined){
    if(el)el.disabled=!en;
    if(wrap){
      wrap.classList.toggle("ctrl-disabled",!en);
      if(wrap.dataset.isRoot==="1"&&Number(wrap.dataset.ctrlType)===2&&wrap.parentElement){
        wrap.parentElement.classList.toggle("panel-disabled",!en);
      }
    }
  }
  const lblVal=(u.l!==undefined)?u.l:(u.op==="lbl"?u.label:undefined);
  if(lblVal!==undefined){
    const lbl=document.getElementById("lbl"+id);
    if(lbl)setCtrlLabelHtml(lbl,lblVal);
    else{
      const hdr=document.getElementById("hdr"+id);
      if(hdr)hdr.innerHTML=lblVal;
    }
  }
  const psVal=(u.ps!==undefined)?u.ps:(u.op==="ps"?u.style:undefined);
  if(psVal!==undefined&&wrap){
    applyCleanStyles(wrap,el,undefined,psVal,undefined,wrap.dataset.isRoot==="1",Number(wrap.dataset.ctrlType||0),wrap.parentElement);
  }
  const esVal=(u.es!==undefined)?u.es:(u.op==="es"?u.style:undefined);
  if(esVal!==undefined&&wrap){
    applyCleanStyles(wrap,el,esVal,undefined,undefined,wrap.dataset.isRoot==="1",Number(wrap.dataset.ctrlType||0),wrap.parentElement);
  }
}

async function loadUiLayout(){
  const res=await fetch("/api/ui");
  const data=await res.json();
  if(data.title)document.getElementById("ui-title").textContent=data.title;
  const nav=document.getElementById("tabs-nav");
  const main=document.getElementById("tabs-content");
  nav.innerHTML="";
  main.innerHTML="";
  const panels={};
  const tabs=[];
  data.controls.forEach(c=>{
    if(c.t===0){
      const btn=document.createElement("button");
      btn.className="tab-btn"+(tabs.length===0?" active":"");
      btn.dataset.tab=c.id;
      btn.innerHTML=c.v||c.l||("Tab "+c.id);
      btn.onclick=()=>activateTab(c.id);
      nav.appendChild(btn);
      const pane=document.createElement("div");
      pane.className="tab-pane"+(tabs.length===0?" active":"");
      pane.id="tab"+c.id;
      main.appendChild(pane);
      tabs.push(c.id);
    }
  });
  data.controls.forEach(c=>{
    if(c.t===0)return;
    if(c.t===8){
      const sel=document.getElementById("select"+c.p);
      if(sel){
        const opt=document.createElement("option");
        opt.value=c.v;
        opt.textContent=c.l;
        if(String(c.v)===sel.dataset.initVal)opt.selected=true;
        sel.appendChild(opt);
      }
      return;
    }
    const parentTab=document.getElementById("tab"+c.p);
    if(c.t===1){
      if(parentTab){
        const sep=document.createElement("div");
        sep.className="separator";
        sep.innerHTML=c.l||c.v||"";
        parentTab.appendChild(sep);
      }
      return;
    }
    if(parentTab||c.p===65535){
      const targetTab=parentTab||main.firstElementChild;
      if(!targetTab)return;
      const panel=document.createElement("div");
      panel.className="panel"+(c.w?" wide":"");
      panel.id="panel"+c.id;
      if(c.ps&&c.ps.includes("display: none"))panel.style.display="none";
      if(c.l){
        const hdr=document.createElement("div");
        hdr.className="panel-hdr";
        hdr.id="hdr"+c.id;
        hdr.innerHTML=c.l;
        panel.appendChild(hdr);
      }
      panel.appendChild(createControlDom(c,true,panel));
      const prevEl=targetTab.lastElementChild;
      if(!c.w&&prevEl&&(c.l==="Installation code"||(c.ps&&c.ps.includes("stack-prev")))){
        if(prevEl.classList.contains("panel")&&!prevEl.classList.contains("wide")){
          const col=document.createElement("div");
          col.className="panel-col";
          targetTab.replaceChild(col,prevEl);
          col.appendChild(prevEl);
          col.appendChild(panel);
        }else if(prevEl.classList.contains("panel-col")){
          prevEl.appendChild(panel);
        }else{
          targetTab.appendChild(panel);
        }
      }else{
        targetTab.appendChild(panel);
      }
      panels[c.id]=panel;
    }else if(panels[c.p]){
      panels[c.p].appendChild(createControlDom(c,false,panels[c.p]));
    }
  });
  window.__z2sUiReady=true;
  connectWebSocket();
}

function connectWebSocket(){
  const proto=location.protocol==="https:"?"wss://":"ws://";
  window.websock=new WebSocket(proto+location.host+"/ws");
  const badge=document.getElementById("ws-status");
  window.websock.onopen=()=>{
    badge.textContent="Connected";
    badge.className="online";
    if(window.__z2sUiReady)setTimeout(()=>sendWs("uicc:1"),200);
  };
  window.websock.onmessage=(evt)=>{
    try{
      const msg=JSON.parse(evt.data);
      if(!msg||typeof msg!=="object")return;
      if(msg.type==="z2s_batch"&&Array.isArray(msg.u))msg.u.forEach(applySingleControlUpdate);
      else if(msg.type==="z2s_upd")applySingleControlUpdate(msg);
      else if(msg.type==="update_select")updateSelectOptions(msg.targets||msg.target,msg.options,msg.selected,msg.clear??true);
      else if(msg.type==="update_action_table")updateActionTable(msg);
      else if(msg.type==="highlight_action_row")highlightActionRow(msg);
      else if(msg.type==="edit_action_row")editActionRow(msg);
      else if(msg.type==="EXEC_JS"){try{(0,eval)(msg.code);}catch(err){console.error(err);}}
      if(window.z2sActionEditActive){updateActionThresholdsVisibility();syncSubactionDropdown();}
    }catch(e){console.error("WS parse error:",e);}
  };
  window.websock.onclose=()=>{
    badge.textContent="Reconnecting...";
    badge.className="";
    setTimeout(connectWebSocket,2000);
  };
}

window.addEventListener("beforeunload",()=>{
  if(window.websock&&window.websock.readyState===WebSocket.OPEN)window.websock.close();
});
window.addEventListener("DOMContentLoaded",loadUiLayout);
</script>
</body>
</html>)rawliteral";

/*****************************************************************************/

#if Z2S_USE_PSRAM
bool Z2S_ESPUIClass::_ensureControlsAllocated() {
  if (_controls) return true;
  void *raw = z2s_gui_malloc(sizeof(BasicControl) * Z2S_MAX_CONTROLS);
  if (!raw) {
    log_e("Failed to allocate _controls pool!");
    return false;
  }
  _controls = static_cast<BasicControl *>(raw);
  for (uint16_t i = 0; i < Z2S_MAX_CONTROLS; i++) {
    new (&_controls[i]) BasicControl();
  }
  return true;
}
#endif

/*****************************************************************************/

void Z2S_ESPUIClass::begin(
  const char *title, const char *username, const char *password, uint16_t port) {

  Z2S_PsramScope psram_scope;
#if Z2S_USE_PSRAM
  _ensureControlsAllocated();
#endif
  // ... existing begin() code ...


  if (title) _ui_title = title;

  if (!_server) {
    _server = new AsyncWebServer(port);
    _ws = new AsyncWebSocket("/ws");

    _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                        AwsEventType type, void *arg, uint8_t *data, size_t len) {
      this->_handleWsEvent(server, client, type, arg, data, len);
    });

    _server->addHandler(_ws);

    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response =
        request->beginResponse_P(200, "text/html", Z2S_INDEX_HTML);
      response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate");
      response->addHeader("Connection", "close");
      request->send(response);
    });

    _server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response = request->beginResponse(204);
      response->addHeader("Connection", "close");
      request->send(response);
    });

    _server->on("/api/ui", HTTP_GET, [this](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response = request->beginChunkedResponse(
        "application/json",
        [this](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
          if (index == 0) {
            s_ui_ctrl_idx = 0;
            s_ui_stage_len = 0;
            s_ui_stage_off = 0;
            s_ui_streaming = true;
          }
          s_ui_stream_ms = millis();
          size_t n = this->_fillUiLayoutChunk(buffer, maxLen, s_ui_ctrl_idx);
          if (n == 0) {
            s_ui_streaming = false;
          }
          return n;
        });
      response->addHeader("Cache-Control", "no-store");
      response->addHeader("Connection", "close");
      request->send(response);
    });

    _server->onNotFound([](AsyncWebServerRequest *request) {
      request->redirect("/");
    });
  }

  _server->begin();
  log_i("Z2S_ESPUI started with %u controls (sizeof(_controls) = %u B)",
        _control_count, (unsigned)sizeof(_controls));
}

/*****************************************************************************/

size_t Z2S_ESPUIClass::_fillUiLayoutChunk(
  uint8_t *buffer, size_t maxLen, size_t &ctrl_idx) {

  if (maxLen == 0) return 0;
  char *out = reinterpret_cast<char *>(buffer);
  size_t out_off = 0;

  while (out_off < maxLen) {
    // 1. Drain any staged bytes that didn't fit in the previous TCP window
    if (s_ui_stage_off < s_ui_stage_len) {
      size_t avail = s_ui_stage_len - s_ui_stage_off;
      size_t room = maxLen - out_off;
      size_t copy_n = (avail < room) ? avail : room;
      memcpy(out + out_off, s_batch_buf + s_ui_stage_off, copy_n);
      out_off += copy_n;
      s_ui_stage_off += copy_n;
      continue;
    }

    // 2. Check if all controls and closing "]}" have been emitted
    if (ctrl_idx > _control_count) {
      break;
    }

    // 3. Format next piece into s_batch_buf (always 1360 B capacity)
    s_ui_stage_len = 0;
    s_ui_stage_off = 0;

    if (ctrl_idx == 0) {
      appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "{\"title\":\"");
      appendEscapedJson(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, _ui_title);
      appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "\",\"controls\":[");
    }

    if (ctrl_idx < _control_count) {
      const BasicControl &c = _controls[ctrl_idx];
      size_t saved_len = s_ui_stage_len;

      bool ok = appendFmt(
        s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len,
        "%s{\"id\":%u,\"t\":%u,\"p\":%u,\"c\":%u,\"e\":%u,\"w\":%u,\"f\":%u,\"r\":%u,\"k\":%u,\"l\":\"",
        (ctrl_idx == 0) ? "" : ",",
        c.id, (uint8_t)c.type, c.parent_id, (uint8_t)c.color,
        c.enabled ? 1 : 0, c.wide_panel ? 1 : 0,
        c.control_flags, c.rows, c.cols);

      ok = ok && appendEscapedJson(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, c.label);
      ok = ok && appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "\",\"v\":\"");

      if (c.str_buf || c.static_str_val) {
        ok = ok && appendEscapedJson(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, c.getValueCstr());
      } else {
        ok = ok && appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "%ld", (long)c.int_val);
      }

      ok = ok && appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "\",\"ps\":\"");
      ok = ok && appendEscapedJson(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, c.panel_style);
      ok = ok && appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "\",\"es\":\"");
      ok = ok && appendEscapedJson(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, c.element_style);
      ok = ok && appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "\"}");

      if (!ok) {
        // Fallback if a control's initial value is > 1300B: send empty value so JSON stays valid
        s_ui_stage_len = saved_len;
        appendFmt(
          s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len,
          "%s{\"id\":%u,\"t\":%u,\"p\":%u,\"c\":%u,\"e\":%u,\"w\":%u,\"f\":%u,\"r\":%u,\"k\":%u,\"l\":\"\",\"v\":\"\",\"ps\":\"\",\"es\":\"\"}",
          (ctrl_idx == 0) ? "" : ",",
          c.id, (uint8_t)c.type, c.parent_id, (uint8_t)c.color,
          c.enabled ? 1 : 0, c.wide_panel ? 1 : 0,
          c.control_flags, c.rows, c.cols);
      }
      ctrl_idx++;
    } else if (ctrl_idx == _control_count) {
      appendFmt(s_batch_buf, sizeof(s_batch_buf), s_ui_stage_len, "]}");
      ctrl_idx++;
    }
  }

  return out_off;
}

/*****************************************************************************/

uint16_t Z2S_ESPUIClass::addControl(
  Control::Type type, const char *label, const char *value,
  Control::Color color, uint16_t parent, BasicControl::Callback_t callback,
  void *user_data) {

#if Z2S_USE_PSRAM
  if (!_ensureControlsAllocated()) return 0xFFFF;
#endif
  if (_control_count >= Z2S_MAX_CONTROLS) {
    log_e("Z2S_MAX_CONTROLS (%u) exceeded!", Z2S_MAX_CONTROLS);
    return 0xFFFF;
  }

  uint16_t idx = _control_count++;
  uint16_t id = idx + 1;

  BasicControl &c = _controls[idx];
  c.id = id;
  c.parent_id = parent;
  c.type = type;
  c.color = color;
  c.label = label ? label : "";
  c.callback = callback;
  c.user_data = user_data;
  c.enabled = true;
  c.wide_panel = false;
  c.dirty_flags = 0;

  if (type == Control::Type::Number || type == Control::Type::Switcher ||
      type == Control::Type::Select || type == Control::Type::Option) {
    c.int_val = value ? strtol(value, nullptr, 10) : 0;
    c.static_str_val = value;
  } else {
    c.static_str_val = value ? value : "";
  }

  return id;
}

uint16_t Z2S_ESPUIClass::addControl(
  Control::Type type, const char *label, const String &value,
  Control::Color color, uint16_t parent, BasicControl::Callback_t callback,
  void *user_data) {

  uint16_t id = addControl(type, label, "", color, parent, callback, user_data);
  BasicControl *c = getControl(id);
  if (c && c->id != 0xFFFF && value.length() > 0) {
    c->setStringValue(value.c_str());
  }
  return id;
}

uint16_t Z2S_ESPUIClass::addControl(
  Control::Type type, const char *label, long int value,
  Control::Color color, uint16_t parent, BasicControl::Callback_t callback,
  void *user_data) {

  uint16_t id = addControl(type, label, nullptr, color, parent, callback, user_data);
  BasicControl *c = getControl(id);
  if (c && c->id != 0xFFFF) {
    c->setIntValue(value); 
  }
  return id;
}

/*****************************************************************************/

BasicControl *Z2S_ESPUIClass::getControl(uint16_t id) {
#if Z2S_USE_PSRAM
  if (!_controls) return &_dummy_ctrl;
#endif
  if (id == 0 || id > _control_count) return &_dummy_ctrl;
  return &_controls[id - 1];
}


/*****************************************************************************/

void Z2S_ESPUIClass::_markDirty(BasicControl *ctrl, uint8_t flag) {
  if (!ctrl || ctrl->id == 0xFFFF) return;
  if (!_ws || _ws->count() == 0) return;
  ctrl->dirty_flags |= flag;
  _has_dirty_controls = true;
}

bool Z2S_ESPUIClass::wsTextAll(const char *payload, size_t len, bool allow_wait) {
  if (!_ws || _ws->count() == 0 || !payload || len == 0) return false;

  if (allow_wait && !_in_ws_event) {
    uint32_t t0 = millis();
    while (!_ws->availableForWriteAll() && (millis() - t0 < 200)) {
      delay(4);
    }
  }

    if (!_ws->availableForWriteAll()) {
    return false;
  }

  Z2S_PsramScope psram_scope;
  _ws->textAll(payload, len);
  return true;

}

void Z2S_ESPUIClass::flush(bool allow_wait) {

  if (!_has_dirty_controls) return;
  if (s_ui_streaming) {
    if ((millis() - s_ui_stream_ms) < 1000) return;
    s_ui_streaming = false;
  }

  if (!_ws || _ws->count() == 0) {
    for (uint16_t i = 0; i < _control_count; i++) {
      _controls[i].dirty_flags = 0;
    }
    _has_dirty_controls = false;
    return;
  }

  char *msg = s_batch_buf;
  constexpr size_t msg_cap = sizeof(s_batch_buf);
  size_t off = 0;
  uint16_t items_in_batch = 0;
  bool any_remaining = false;

  for (uint16_t i = 0; i < _control_count; i++) {
    BasicControl &c = _controls[i];
    if (c.dirty_flags == 0) continue;

    if (items_in_batch == 0) {
      off = 0;
      appendFmt(msg, msg_cap, off, "{\"type\":\"z2s_batch\",\"u\":[");
    }

    size_t saved_off = off;
    uint8_t flags = c.dirty_flags;

    bool ok = appendFmt(msg, msg_cap, off, "%s{\"id\":%u",
                        (items_in_batch == 0) ? "" : ",", c.id);

    if (ok && (flags & Z2S_DIRTY_VAL)) {
      ok = appendFmt(msg, msg_cap, off, ",\"v\":\"");
      if (c.str_buf || c.static_str_val) {
        ok = ok && appendEscapedJson(msg, msg_cap, off, c.getValueCstr());
      } else {
        ok = ok && appendFmt(msg, msg_cap, off, "%ld", (long)c.int_val);
      }
      ok = ok && appendFmt(msg, msg_cap, off, "\"");
    }

    if (ok && (flags & Z2S_DIRTY_EN)) {
      ok = appendFmt(msg, msg_cap, off, ",\"e\":%u", c.enabled ? 1 : 0);
    }

    if (ok && (flags & Z2S_DIRTY_LBL)) {
      ok = appendFmt(msg, msg_cap, off, ",\"l\":\"") &&
           appendEscapedJson(msg, msg_cap, off, c.label) &&
           appendFmt(msg, msg_cap, off, "\"");
    }

    if (ok && (flags & Z2S_DIRTY_PS)) {
      ok = appendFmt(msg, msg_cap, off, ",\"ps\":\"") &&
           appendEscapedJson(msg, msg_cap, off, c.panel_style) &&
           appendFmt(msg, msg_cap, off, "\"");
    }

    if (ok && (flags & Z2S_DIRTY_ES)) {
      ok = appendFmt(msg, msg_cap, off, ",\"es\":\"") &&
           appendEscapedJson(msg, msg_cap, off, c.element_style) &&
           appendFmt(msg, msg_cap, off, "\"");
    }

    ok = ok && appendFmt(msg, msg_cap, off, "}");

    if (!ok || (msg_cap - off) < 4) {
      off = saved_off;
      if (items_in_batch > 0) {
        appendFmt(msg, msg_cap, off, "]}");
        if (!wsTextAll(msg, off, allow_wait)) {
          any_remaining = true;
          break;
        }
        items_in_batch = 0;
        i--;
        continue;
      } else {
        c.dirty_flags = 0;
        continue;
      }
    }

    c.dirty_flags = 0;
    items_in_batch++;
  }

  if (items_in_batch > 0) {
    appendFmt(msg, msg_cap, off, "]}");
    wsTextAll(msg, off, allow_wait);
  }

  _has_dirty_controls = any_remaining;
}

void Z2S_ESPUIClass::loop() {
  _last_loop_ms = millis();

  WsQueuedEvent ev;
  while (dequeueWsEvent(ev)) {
    if (ev.is_custom) {
      if (_on_ws_custom_cmd) {
        _on_ws_custom_cmd(ev.id);
      }
    } else {
      BasicControl *ctrl = getControl(ev.id);
      if (ctrl && ctrl->id != 0xFFFF && ctrl->callback) {
        ctrl->callback(ctrl, ev.cb_type, ctrl->user_data);
      }
    }
  }

  flush(true);
}

/*****************************************************************************/

void Z2S_ESPUIClass::updateControlValue(uint16_t id, const char *value) {
  
  BasicControl *c = getControl(id);
  if (!c || c->id == 0xFFFF) return;
  
  const char *new_val = value ? value : "";
  
  if ((c->str_buf || c->static_str_val) && 
       strcmp(c->getValueCstr(), new_val) == 0) {
    return;
  }
  c->setStringValue(new_val);
  _markDirty(c, Z2S_DIRTY_VAL);
}

void Z2S_ESPUIClass::updateControlValue(uint16_t id, const String &value) {
  updateControlValue(id, value.c_str());
}

void Z2S_ESPUIClass::updateControlValue(uint16_t id, long int value) {

  BasicControl *c = getControl(id);
  
  if (!c || c->id == 0xFFFF) return;
  
  if (!c->str_buf && !c->static_str_val && c->int_val == value) {
    return;
  }
  c->setIntValue(value);
  _markDirty(c, Z2S_DIRTY_VAL);
}

void Z2S_ESPUIClass::updateControlLabel(uint16_t id, const char *label) {
  BasicControl *c = getControl(id);
  if (!c || c->id == 0xFFFF) return;
  const char *new_lbl = label ? label : "";
  if (c->label && strcmp(c->label, new_lbl) == 0) return;
  c->setLabel(new_lbl, false);
  _markDirty(c, Z2S_DIRTY_LBL);
}

void Z2S_ESPUIClass::setEnabled(uint16_t id, bool enable) {
  BasicControl *c = getControl(id);
  if (!c || c->id == 0xFFFF || c->enabled == enable) return;
  c->enabled = enable;
  _markDirty(c, Z2S_DIRTY_EN);
}

void Z2S_ESPUIClass::setPanelStyle(uint16_t id, const char *style) {
  BasicControl *c = getControl(id);
  if (!c || c->id == 0xFFFF) return;
  const char *new_ps = style ? style : "";
  if (c->panel_style && strcmp(c->panel_style, new_ps) == 0) return;
  c->panel_style = new_ps;
  _markDirty(c, Z2S_DIRTY_PS);
}

void Z2S_ESPUIClass::setElementStyle(uint16_t id, const char *style) {
  BasicControl *c = getControl(id);
  if (!c || c->id == 0xFFFF) return;
  const char *new_es = style ? style : "";
  if (c->element_style && strcmp(c->element_style, new_es) == 0) return;
  c->element_style = new_es;
  _markDirty(c, Z2S_DIRTY_ES);
}

void Z2S_ESPUIClass::setPanelWide(uint16_t id, bool wide) {
  BasicControl *c = getControl(id);
  if (c && c->id != 0xFFFF) c->wide_panel = wide;
}

void Z2S_ESPUIClass::setInputType(uint16_t id, const char *input_type) {
  BasicControl *c = getControl(id);
  if (c && c->id != 0xFFFF && input_type && strcmp(input_type, "password") == 0) {
    c->control_flags |= CONTROL_FLAG_PASSWORD;
  }
}

/*****************************************************************************/

void Z2S_ESPUIClass::_handleWsEvent(
  AsyncWebSocket *server, AsyncWebSocketClient *client,
  AwsEventType type, void *arg, uint8_t *data, size_t len) {

  if (type == WS_EVT_CONNECT) {
    server->cleanupClients(5);
    if (_on_ws_connect) _on_ws_connect();
    return;
  }

  if (type != WS_EVT_DATA || !data || len == 0) return;

  AwsFrameInfo *info = static_cast<AwsFrameInfo *>(arg);
  if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) {
    return;
  }

  const char *raw = reinterpret_cast<const char *>(data);
  const char *colon1 = static_cast<const char *>(memchr(raw, ':', len));
  if (!colon1) return;

  size_t cmd_len = (size_t)(colon1 - raw);
  if (cmd_len == 0 || cmd_len >= 16) return;

  char cmd[16];
  memcpy(cmd, raw, cmd_len);
  cmd[cmd_len] = '\0';

  const char *rest = colon1 + 1;
  size_t rest_len = len - cmd_len - 1;

  if (strcmp(cmd, "uicc") == 0) {
    char id_buf[16];
    size_t copy_n = (rest_len < 15) ? rest_len : 15;
    memcpy(id_buf, rest, copy_n);
    id_buf[copy_n] = '\0';
    uint16_t custom_cmd = (uint16_t)strtoul(id_buf, nullptr, 10);
    enqueueWsEvent(custom_cmd, 0, 1);
    return;
  }

  const char *colon2 = static_cast<const char *>(memchr(rest, ':', rest_len));
  size_t id_len = colon2 ? (size_t)(colon2 - rest) : rest_len;
  if (id_len == 0 || id_len >= 16) return;

  char id_buf[16];
  memcpy(id_buf, rest, id_len);
  id_buf[id_len] = '\0';

  uint16_t id = (uint16_t)strtoul(id_buf, nullptr, 10);
  BasicControl *ctrl = getControl(id);
  if (!ctrl || ctrl->id == 0xFFFF) return;

  const char *val_ptr = colon2 ? (colon2 + 1) : "";
  size_t val_len = colon2 ? (rest_len - id_len - 1) : 0;

  int8_t cb_type = 0;
  if (strcmp(cmd, "bdown") == 0) {
    cb_type = B_DOWN;
  } else if (strcmp(cmd, "bup") == 0) {
    cb_type = B_UP;
  } else if (strcmp(cmd, "sactive") == 0) {
    ctrl->int_val = 1;
    cb_type = S_ACTIVE;
  } else if (strcmp(cmd, "sinactive") == 0) {
    ctrl->int_val = 0;
    cb_type = S_INACTIVE;
  } else if (strcmp(cmd, "nvalue") == 0 || strcmp(cmd, "slvalue") == 0) {
    char num_buf[24];
    size_t n = (val_len < 23) ? val_len : 23;
    memcpy(num_buf, val_ptr, n);
    num_buf[n] = '\0';
    ctrl->setIntValue(strtol(num_buf, nullptr, 10));
    cb_type = (cmd[0] == 'n') ? N_VALUE : S_VALUE;
  } else if (strcmp(cmd, "tvalue") == 0) {
    setControlStringLen(ctrl, val_ptr, val_len);
    cb_type = T_VALUE;
  } else if (strcmp(cmd, "tabvalue") == 0) {
    cb_type = 0;
  } else {
    return;
  }

  // If Z2S_loopWebGUI() is actively polling ESPUI.loop(), dispatch on loopTask
  if ((millis() - _last_loop_ms) <= 500) {
    enqueueWsEvent(id, cb_type, 0);
  } else {
    _in_ws_event = true;
    if (ctrl->callback) {
      ctrl->callback(ctrl, cb_type, ctrl->user_data);
    }
    _in_ws_event = false;
    flush(false);
  }
}
