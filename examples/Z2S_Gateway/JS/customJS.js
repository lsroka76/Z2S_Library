window.z2sIrOptions = [];
window.z2sPoOptions = [];

function redirect2ZigbeeOTA() {
  window.location.assign("/zigbee_ota");
}

window.onGatewayCommand = function(command, labelId) {
  console.log("Received gateway command:", command);
  switch (command) {
    case "Z2S_GUI_CMD_START_BACKUP":
      $("#l" + labelId).html("Downloading...");
      (async () => {
        await downloadPartition("nvs", null);
        await downloadPartition("spiffs", null);
        await downloadPartition("zb_storage", null);
      })();
      break;
    case "Z2S_GUI_CMD_UI_LOCKDOWN":
      document.body.style.pointerEvents = "none";
      document.body.style.opacity = "0.5";
      break;
    case "CMD_UI_UNLOCK":
      document.body.style.pointerEvents = "auto";
      document.body.style.opacity = "1";
      break;
    default:
      console.warn("Unknown command:", command);
      break;
  }
};

function updateSelectOptions(targetSelects, optionsData, selectedValue = null, clearExisting = true) {
  const targets = Array.isArray(targetSelects) ? targetSelects : [targetSelects];
  let optionsList = [];
  if (Array.isArray(optionsData)) {
    optionsList = optionsData.map(item => Array.isArray(item)
      ? { value: item[0], label: item[1], selected: Boolean(item[2]) }
      : { value: item.value ?? item.val ?? item.id, label: item.label ?? item.name ?? item.text, selected: Boolean(item.selected ?? item.isSelected) }
    );
  } else if (typeof optionsData === 'object' && optionsData !== null) {
    optionsList = Object.entries(optionsData).map(([value, label]) => ({ value, label, selected: false }));
  }
  const targetSelectedStr = (selectedValue !== null && selectedValue !== undefined) ? String(selectedValue) : null;

  targets.forEach(target => {
    let selectEl = (typeof target === 'string')
      ? (document.getElementById(target) || document.querySelector(`select[name="${target}"]`) || document.querySelector(target))
      : (target instanceof HTMLElement ? target : null);

    if (!selectEl) {
      setTimeout(() => updateSelectOptions([target], optionsData, selectedValue, clearExisting), 150);
      return;
    }
    if (clearExisting) selectEl.innerHTML = '';
    const fragment = document.createDocumentFragment();
    optionsList.forEach(({ value, label, selected }) => {
      const option = document.createElement('option');
      option.value = value;
      option.textContent = label;
      if (targetSelectedStr !== null) {
        if (String(value) === targetSelectedStr) option.selected = true;
      } else if (selected) {
        option.selected = true;
      }
      fragment.appendChild(option);
    });
    selectEl.appendChild(fragment);
    selectEl.dispatchEvent(new Event('change', { bubbles: true }));
  });
}

function sendActionClickToESP32(actionIndex) {
  const hiddenInput = document.getElementById('num2');
  if (hiddenInput) {
    hiddenInput.value = actionIndex;
    numberchange(2);
  }
}

function formatActionTableCell(cellContent, colIdx, isIr, isPo) {
  const str = String(cellContent);
  if (isIr || isPo) return str;
  if (colIdx === 0) {
    const lower = str.toLowerCase();
    if (lower.endsWith(" (on)")) {
      return `<span class="action-name-cell"><span class="action-btn-on">ON</span><span class="action-name-text">${str.slice(0, -5)}</span></span>`;
    }
    if (lower.endsWith(" (off)")) {
      return `<span class="action-name-cell"><span class="action-btn-off">OFF</span><span class="action-name-text">${str.slice(0, -6)}</span></span>`;
    }
    return `<span class="action-name-cell"><span class="action-name-text">${str}</span></span>`;
  }
  if (colIdx === 1 || colIdx === 3) {
    let metaIdx = str.indexOf(" (v:");
    if (metaIdx === -1) metaIdx = str.indexOf(" (sid:");
    if (metaIdx !== -1 && str.endsWith(")")) {
      return `<span class="action-flat-btn">${str.slice(0, metaIdx)}</span><span class="action-btn-meta">${str.slice(metaIdx)}</span>`;
    }
    return `<span class="action-flat-btn">${str}</span>`;
  }
  if (colIdx === 2 || colIdx === 4) {
    const safeChannel = str.split('"').join('&quot;');
    return `<span class="channel-cell-wrap"><span class="channel-chain-btn" data-channel="${safeChannel}" title="Filter actions linked to this channel">&#128279;</span><span class="channel-cell-name">${str}</span></span>`;
  }
  return str;
}

function filterActionTable(targetId) {
  const targetLabel = document.getElementById(targetId);
  if (!targetLabel) return;
  const searchInput = targetLabel.querySelector(".table-search-input");
  const clearBtn = targetLabel.querySelector(".table-search-clear");
  const countBadge = targetLabel.querySelector(".table-search-count");
  const tbody = targetLabel.querySelector("tbody");
  if (!searchInput || !tbody) return;

  const query = searchInput.value.trim().toLowerCase();
  targetLabel.dataset.searchQuery = searchInput.value;
  if (clearBtn) clearBtn.style.display = query.length > 0 ? "inline-flex" : "none";

  const isIr = targetLabel.dataset.isIr === "1";
  const isPo = targetLabel.dataset.isPo === "1";
  const colSpan = (isIr || isPo) ? 3 : 5;
  const searchCols = (isIr || isPo) ? [1] : [0, 2, 4];
  const rows = tbody.querySelectorAll("tr[data-action-idx]");
  let visibleCount = 0;

  rows.forEach(row => {
    const cells = row.querySelectorAll("td");
    let rowMatches = !query;
    searchCols.forEach(colIdx => {
      const cell = cells[colIdx];
      if (!cell) return;
      let rawText = cell.getAttribute("title") || cell.textContent || "";
      if (!isIr && !isPo && colIdx === 0) {
        const lr = rawText.toLowerCase();
        if (lr.endsWith(" (on)")) rawText = rawText.slice(0, -5);
        else if (lr.endsWith(" (off)")) rawText = rawText.slice(0, -6);
      }
      const colHit = Boolean(query) && rawText.toLowerCase().includes(query);
      if (colHit) rowMatches = true;
      if (!isIr && !isPo && (colIdx === 2 || colIdx === 4)) {
        const wrap = cell.querySelector(".channel-cell-wrap");
        if (wrap) {
          wrap.classList.toggle("chain-match-src", colHit && colIdx === 2);
          wrap.classList.toggle("chain-match-dst", colHit && colIdx === 4);
        }
      }
    });
    row.style.display = rowMatches ? "" : "none";
    if (rowMatches) visibleCount++;
  });

  let noMatchRow = tbody.querySelector("tr.no-search-match");
  if (rows.length > 0 && visibleCount === 0) {
    if (!noMatchRow) {
      noMatchRow = document.createElement("tr");
      noMatchRow.className = "no-search-match";
      noMatchRow.innerHTML = `<td colspan="${colSpan}" style="text-align:center;padding:16px;color:#94a3b8;font-style:italic;">No matching items found</td>`;
      tbody.appendChild(noMatchRow);
    }
    noMatchRow.style.display = "";
  } else if (noMatchRow) {
    noMatchRow.style.display = "none";
  }
  if (countBadge) countBadge.textContent = query ? `${visibleCount} / ${rows.length}` : `${rows.length}`;
}

function updateActionTable(data) {
  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;
  const isIr = Boolean(data.is_ir);
  const isPo = Boolean(data.is_po);
  const colSpan = (isIr || isPo) ? 3 : 5;
  const emptyMsg = isIr ? "No IR codes saved for this device" : (isPo ? "No Pushover messages available" : "No actions available");
  const searchPlaceholder = isIr ? "Search IR command by name..." : (isPo ? "Search Pushover message by name..." : "Search by action, source, or destination channel...");

  targetLabel.dataset.isIr = isIr ? "1" : "0";
  targetLabel.dataset.isPo = isPo ? "1" : "0";
  const savedSearch = targetLabel.dataset.searchQuery || "";
  const isExpanded = targetLabel.dataset.isExpanded === "1";
  let rowsHtml = "";

  if (!data.d || data.d.length === 0) {
    rowsHtml = `<tr><td colspan="${colSpan}" style="text-align:center;padding:16px;color:#6c757d;font-style:italic;">${emptyMsg}</td></tr>`;
  } else {
    if (!data.append) {
      if (isIr) window.z2sIrOptions = [];
      if (isPo) window.z2sPoOptions = [];
    }
    data.d.forEach((row, index) => {
      if ((isIr || isPo) && row.length >= 2) {	
        const slotId = parseInt(String(row[0]).replace('#', ''), 10);
        if (!isNaN(slotId)) {
          const entry = { id: slotId, label: `${row[0]} - ${row[1]}` };
          if (isIr) window.z2sIrOptions.push(entry);
          if (isPo) window.z2sPoOptions.push(entry);
        }
      }
      const absoluteIndex = (data.start_idx !== undefined ? data.start_idx : 0) + index;
      const style = (absoluteIndex === data.s) ? "background-color:rgba(0,123,255,0.2);font-weight:bold;" : "font-weight:normal;";
      rowsHtml += `<tr style="${style}" data-action-idx="${absoluteIndex}">`;
      row.forEach((cell, colIdx) => {
        const cleanCell = String(cell).split('"').join('&quot;');
        rowsHtml += `<td style="padding:3px 6px;border-bottom:1px solid #eee;vertical-align:middle;" title="${cleanCell}">${formatActionTableCell(cell, colIdx, isIr, isPo)}</td>`;
      });
      rowsHtml += "</tr>";
    });
  }

  if (data.append) {
    const tbody = targetLabel.querySelector("tbody");
    if (tbody) {
      tbody.insertAdjacentHTML('beforeend', rowsHtml);
      filterActionTable(data.target);
    }
    return;
  }

  const T = `#${data.target}`;
  let html = `<style>` +
    `${T} tbody tr[data-action-idx]{cursor:pointer;transition:background .15s}` +
    `${T} tbody tr[data-action-idx]:hover{background-color:rgba(0,123,255,.2)!important}` +
    `${T} th{position:sticky;top:0;background:#e9ecef;color:#212529;font-weight:600;border-bottom:2px solid #adb5bd;z-index:10;box-shadow:0 2px 4px -2px rgba(0,0,0,.25)}` +
    `${T} th,${T} td{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}` +
    `${T} td{font-size:11px}` +
    `${T} .table-search-bar{display:flex;align-items:center;gap:8px;margin-top:8px;padding:5px 10px;background:rgba(0,0,0,.25);border:1px solid rgba(128,128,128,.25);border-bottom:none;border-radius:6px 6px 0 0}` +
    `${T} .table-search-input{flex:1;background:rgba(255,255,255,.08)!important;border:1px solid rgba(255,255,255,.2)!important;border-radius:4px!important;color:#f8fafc!important;padding:4px 8px!important;font-size:12px!important;outline:none!important;margin:0!important;height:26px!important;box-sizing:border-box!important}` +
    `${T} .table-search-input::placeholder{color:#94a3b8}` +
    `${T} .table-search-input:focus{border-color:#80bdff!important;box-shadow:0 0 0 2px rgba(0,123,255,.25)!important}` +
    `${T} .table-search-clear,${T} .table-expand-btn{display:inline-flex;align-items:center;justify-content:center;background:rgba(255,255,255,.12);border:1px solid rgba(255,255,255,.2);color:#cbd5e1;border-radius:4px;height:24px;font-size:11px;font-weight:600;cursor:pointer;user-select:none;flex-shrink:0}` +
    `${T} .table-search-clear{display:none;padding:0 7px}` +
    `${T} .table-search-clear:hover{background:rgba(231,76,60,.7);color:#fff}` +
    `${T} .table-expand-btn{width:24px;font-size:12px}` +
    `${T} .table-expand-btn:hover{background:rgba(0,123,255,.4);color:#fff;border-color:#80bdff}` +
    `${T} .table-search-count{font-size:11px;font-weight:600;color:#94a3b8;min-width:36px;text-align:right;user-select:none}` +
    `${T} .action-name-cell{display:inline-flex;align-items:center;gap:6px;font-size:11px;line-height:1.3;max-width:100%}` +
    `${T} .action-name-text,${T} .channel-cell-name{font-size:11px;line-height:1.3;overflow:hidden;text-overflow:ellipsis}` +
    `${T} .action-btn-on,${T} .action-btn-off{display:inline-flex;align-items:center;justify-content:center;padding:2px 6px;font-size:10px;font-weight:700;line-height:1.1;border-radius:4px;letter-spacing:.4px;flex-shrink:0}` +
    `${T} .action-btn-on{background:#2ecc71;color:#fff;border:1px solid #27ae60}` +
    `${T} .action-btn-off{background:#e74c3c;color:#fff;border:1px solid #c0392b}` +
    `${T} .action-flat-btn{display:inline-block;padding:1px 6px;font-size:11px;font-weight:600;line-height:1.2;border-radius:4px;background:rgba(128,128,128,.15);border:1px solid rgba(128,128,128,.3);color:inherit}` +
    `${T} .action-btn-meta{font-size:11px;opacity:.8;margin-left:3px}` +
    `${T} .channel-cell-wrap{display:inline-flex;align-items:center;gap:5px;padding:1px 5px;border-radius:4px;max-width:100%;transition:all .15s}` +
    `${T} .channel-chain-btn{display:inline-flex;align-items:center;justify-content:center;font-size:10px;padding:1px 4px;border-radius:3px;background:rgba(128,128,128,.15);border:1px solid rgba(128,128,128,.3);cursor:pointer;opacity:.65;flex-shrink:0;transition:all .15s}` +
    `${T} .channel-chain-btn:hover{opacity:1;background:rgba(0,123,255,.35);border-color:#80bdff;transform:scale(1.08)}` +
    `${T} .chain-match-src{background:rgba(59,130,246,.22);border:1px solid rgba(96,165,250,.55);color:#93c5fd;font-weight:600}` +
    `${T} .chain-match-dst{background:rgba(245,158,11,.22);border:1px solid rgba(251,191,36,.55);color:#fcd34d;font-weight:600}` +
    `${T} div[tabindex]{height:210px;overflow:auto;overscroll-behavior:contain;border:1px solid rgba(128,128,128,.25);transition:height .25s ease,border-color .15s,box-shadow .15s}` +
    `${T} div[tabindex].table-fullscreen{height:70vh!important}` +
    `${T} div[tabindex]::-webkit-scrollbar{width:6px;height:6px}` +
    `${T} div[tabindex]::-webkit-scrollbar-track{background:rgba(128,128,128,.1);border-radius:4px}` +
    `${T} div[tabindex]::-webkit-scrollbar-thumb{background:rgba(128,128,128,.35);border-radius:4px}` +
    `${T} div[tabindex="0"]:focus{border-color:#80bdff!important;box-shadow:0 0 0 .2rem rgba(0,123,255,.25)!important;outline:none!important}` +
    `${T} .table-footer-hints{display:flex!important;flex-wrap:wrap!important;gap:14px!important;justify-content:center!important;align-items:center!important;background:rgba(0,0,0,.3)!important;border:1px solid rgba(255,255,255,.15)!important;border-top:none!important;border-radius:0 0 6px 6px!important;padding:5px 10px!important;font-size:10px!important;line-height:1!important;color:#cbd5e1!important;user-select:none!important}` +
    `${T} .table-footer-hints span{display:inline-flex!important;align-items:center!important;gap:4px!important;font-size:10px!important;line-height:1!important}` +
    `${T} .table-footer-hints kbd{background:#334155!important;border:1px solid #475569!important;border-radius:3px!important;color:#f8fafc!important;display:inline-flex!important;align-items:center!important;justify-content:center!important;font-family:inherit!important;font-size:9px!important;font-weight:600!important;padding:2px 4px!important;line-height:1!important;height:16px!important}` +
    `${T} .table-disabled{pointer-events:none!important;overflow-y:hidden!important;opacity:.5!important;filter:grayscale(50%);user-select:none}` +
    `${T} .table-status-badge{display:inline-block;padding:2px 8px;border-radius:12px;font-size:.7rem;font-weight:600;text-transform:uppercase;letter-spacing:.5px}` +
    `${T} .badge-active{background:#e6f4ea;color:#137333;border:1px solid #ceedd5}` +
    `${T} .badge-locked{background:#fce8e6;color:#c5221f;border:1px solid #f5c6cb}` +
  `</style>`;

  const headerHtml = isIr
    ? "<th style='padding:5px 6px;width:80px;min-width:80px;'>ID (sid)</th><th style='padding:5px 6px;width:220px;min-width:180px;'>Command Name</th><th style='padding:5px 6px;width:420px;min-width:260px;'>IR Code (Base64)</th>"
    : isPo
    ? "<th style='padding:5px 6px;width:80px;min-width:80px;'>ID (sid)</th><th style='padding:5px 6px;width:220px;min-width:180px;'>Message Name</th><th style='padding:5px 6px;width:420px;min-width:260px;'>Message Text</th>"
    : "<th style='padding:5px 6px;width:135px;min-width:135px;'>Name (state)</th><th style='padding:5px 6px;width:135px;min-width:135px;'>Supla event</th><th style='padding:5px 6px;width:180px;min-width:180px;'>Source channel</th><th style='padding:5px 6px;width:120px;min-width:120px;'>Supla action</th><th style='padding:5px 6px;width:180px;min-width:180px;'>Destination channel</th>";

  const safeSavedSearch = savedSearch.split('"').join('&quot;');
  html += `<div class="table-search-bar">` +
    `<input type="text" class="table-search-input" placeholder="\uD83D\uDD0D ${searchPlaceholder}" value="${safeSavedSearch}" autocomplete="off">` +
    `<span class="table-search-clear" title="Clear search">&#10005;</span>` +
    `<span class="table-search-count">0</span>` +
    `<span class="table-expand-btn" title="${isExpanded ? 'Collapse table height' : 'Expand table height'}">${isExpanded ? '&#10529;' : '&#10530;'}</span>` +
  `</div>` +
  `<div tabindex="0" class="${isExpanded ? 'table-fullscreen' : ''}">` +
    `<table style="width:100%;min-width:750px;table-layout:fixed;border-collapse:collapse;text-align:left;">` +
      `<thead><tr>${headerHtml}</tr></thead><tbody>${rowsHtml}</tbody>` +
    `</table>` +
  `</div>` +
  `<div class="table-footer-hints">` +
    `<span><span class="table-status-badge badge-active">Active</span></span>` +
    `<span><kbd>&uarr;</kbd><kbd>&darr;</kbd> Navigate</span>` +
    `<span><kbd>PgUp</kbd><kbd>PgDn</kbd> &plusmn;5 Rows</span>` +
    `<span><kbd>Home</kbd><kbd>End</kbd> First/Last</span>` +
  `</div>`;

  targetLabel.innerHTML = html;
  targetLabel.style.display = "block";

  const searchInput = targetLabel.querySelector(".table-search-input");
  const clearBtn = targetLabel.querySelector(".table-search-clear");
  const expandBtn = targetLabel.querySelector(".table-expand-btn");
  const container = targetLabel.querySelector("div[tabindex='0']");
  const tbody = targetLabel.querySelector("tbody");

  if (searchInput) {
    searchInput.addEventListener("input", () => filterActionTable(data.target));
    searchInput.addEventListener("keydown", (e) => {
      if (e.key === "Escape") {
        searchInput.value = "";
        filterActionTable(data.target);
      } else if (e.key === "ArrowDown" && container) {
        e.preventDefault();
        container.focus();
      }
    });
  }
  if (clearBtn && searchInput) {
    clearBtn.addEventListener("click", () => {
      searchInput.value = "";
      filterActionTable(data.target);
      searchInput.focus();
    });
  }
  if (expandBtn && container) {
    expandBtn.addEventListener("click", () => {
      const isFull = container.classList.toggle("table-fullscreen");
      targetLabel.dataset.isExpanded = isFull ? "1" : "0";
      expandBtn.innerHTML = isFull ? "&#10529;" : "&#10530;";
      expandBtn.title = isFull ? "Collapse table height" : "Expand table height";
    });
  }

  filterActionTable(data.target);

  if (tbody && container) {
    const activateRow = (rowElement, shouldScroll = true) => {
      if (!rowElement) return;
      const actionIndex = parseInt(rowElement.getAttribute("data-action-idx"));
      if (isNaN(actionIndex)) return;
      highlightActionRow({ target: data.target, s: actionIndex }, shouldScroll);
      const clickMask = isIr ? 0x4000 : (isPo ? 0x2000 : 0);
      sendActionClickToESP32(actionIndex | clickMask);
    };

    tbody.addEventListener("click", function(e) {
      const chainBtn = e.target.closest(".channel-chain-btn");
      if (chainBtn && searchInput) {
        e.stopPropagation();
        const chName = chainBtn.getAttribute("data-channel") || "";
        searchInput.value = (searchInput.value.trim().toLowerCase() === chName.trim().toLowerCase()) ? "" : chName;
        filterActionTable(data.target);
        return;
      }
      const clickedRow = e.target.closest("tr[data-action-idx]");
      if (!clickedRow) return;
      container.focus();
      activateRow(clickedRow, false);
    });

    container.addEventListener("keydown", function(e) {
      const navKeys = ["ArrowUp", "ArrowDown", "Home", "End", "PageUp", "PageDown", "Enter", " "];
      if (!navKeys.includes(e.key)) return;
      const visibleRows = Array.from(tbody.querySelectorAll("tr[data-action-idx]")).filter(r => r.style.display !== "none");
      if (!visibleRows.length) return;
      let currentIndex = visibleRows.findIndex(r => r.style.fontWeight === "bold");
      if (currentIndex === -1) currentIndex = 0;

      if (e.key === "Enter" || e.key === " ") {
        e.preventDefault();
        visibleRows[currentIndex].click();
        return;
      }
      let nextIndex = currentIndex;
      if (e.key === "ArrowUp") nextIndex = Math.max(0, currentIndex - 1);
      else if (e.key === "ArrowDown") nextIndex = Math.min(visibleRows.length - 1, currentIndex + 1);
      else if (e.key === "PageUp") nextIndex = Math.max(0, currentIndex - 5);
      else if (e.key === "PageDown") nextIndex = Math.min(visibleRows.length - 1, currentIndex + 5);
      else if (e.key === "Home") nextIndex = 0;
      else if (e.key === "End") nextIndex = visibleRows.length - 1;

      if (nextIndex !== currentIndex) {
        e.preventDefault();
        activateRow(visibleRows[nextIndex], true);
      }
    });
  }
}

function setTableDisabled(targetId, isDisabled) {
  const targetLabel = document.getElementById(targetId);
  if (!targetLabel) return;
  if (targetLabel.dataset.isIr !== "1" && targetLabel.dataset.isPo !== "1") {
    window.z2sActionEditActive = Boolean(isDisabled);
  }
  const container = targetLabel.querySelector("div[tabindex]");
  const searchBar = targetLabel.querySelector(".table-search-bar");
  const searchInput = targetLabel.querySelector(".table-search-input");
  const badge = targetLabel.querySelector(".table-status-badge");

  if (isDisabled) {
    if (container) { container.classList.add("table-disabled"); container.setAttribute("tabindex", "-1"); container.blur(); }
    if (searchBar) searchBar.classList.add("table-disabled");
    if (searchInput) searchInput.disabled = true;
    if (badge) { badge.textContent = "Locked"; badge.className = "table-status-badge badge-locked"; }
  } else {
    if (container) { container.classList.remove("table-disabled"); container.setAttribute("tabindex", "0"); }
    if (searchBar) searchBar.classList.remove("table-disabled");
    if (searchInput) searchInput.disabled = false;
    if (badge) { badge.textContent = "Active"; badge.className = "table-status-badge badge-active"; }
  }
  [50, 150, 300].forEach(delay => {
    setTimeout(() => {
      if (typeof updateActionThresholdsVisibility === "function") updateActionThresholdsVisibility();
      if (typeof syncSubactionDropdown === "function") syncSubactionDropdown();
    }, delay);
  });
}

function highlightActionRow(data, shouldScroll = true) {
  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;
  const rows = targetLabel.querySelectorAll("tbody tr[data-action-idx]");
  rows.forEach((row) => {
    const rowIdx = parseInt(row.getAttribute("data-action-idx"));
    if (rowIdx === data.s) {
      row.style.backgroundColor = "rgba(0, 123, 255, 0.2)";
      row.style.fontWeight = "bold";
      if (shouldScroll && row.style.display !== "none") {
        row.scrollIntoView({ behavior: "smooth", block: "nearest" });
      }
    } else {
      row.style.backgroundColor = "";
      row.style.fontWeight = "normal";
    }
  });
}

function editActionRow(data) {
  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;
  const isIr = targetLabel.dataset.isIr === "1";
  const isPo = targetLabel.dataset.isPo === "1";
  const rows = targetLabel.querySelectorAll("tbody tr[data-action-idx]");

  if (data.idx < rows.length) {
    const cells = rows[data.idx].querySelectorAll("td");
    data.d.forEach((newCellContent, i) => {
      if (cells[i]) {
        cells[i].innerHTML = formatActionTableCell(newCellContent, i, isIr, isPo);
        cells[i].title = String(newCellContent);
      }
    });
    if ((isIr || isPo) && data.d && data.d.length >= 2) {
      const slotId = parseInt(String(data.d[0]).replace('#', ''), 10);
      const newLabel = `${data.d[0]} - ${data.d[1]}`;
      const targetList = isIr ? window.z2sIrOptions : window.z2sPoOptions;
      if (targetList && !isNaN(slotId)) {
        const item = targetList.find(opt => opt.id === slotId);
        if (item) item.label = newLabel;
        else targetList.push({ id: slotId, label: newLabel });
        syncSubactionDropdown();
      }
    }
    filterActionTable(data.target);
  }
}

const originalParse = JSON.parse;
JSON.parse = function(text, reviver) {
  const data = originalParse(text, reviver);
  if (data && typeof data === 'object') {
    if (data.type === "EXEC_JS") {
      try { eval(data.code); } catch (err) { console.error(err); }
      return { type: -999, id: -1, value: "" };
    }
    if (data.type === 'update_select') {
      updateSelectOptions(data.targets || data.target, data.options, data.selected, data.clear ?? true);
      return { type: -999, id: -1, value: "" };
    }
    if (data.type === 'update_action_table') {
      updateActionTable(data);
      return { type: -998, id: -1, value: "" };
    }
    if (data.type === 'highlight_action_row') {
      highlightActionRow(data);
      return { type: -997, id: -1, value: "" };
    }
    if (data.type === 'edit_action_row') {
      editActionRow(data);
      return { type: -999, id: -1, value: "" };
    }
  }
  if (window.z2sActionEditActive) {
    clearTimeout(window.z2sSyncTimer);
    window.z2sSyncTimer = setTimeout(() => {
      if (typeof updateActionThresholdsVisibility === "function") updateActionThresholdsVisibility();
      if (typeof syncSubactionDropdown === "function") syncSubactionDropdown();
    }, 30);
  }
  return data;
};
console.log("JSON.parse hook successfully installed!");

async function downloadPartition(label, btn) {
  if (btn) btn.innerText = `Downloading ${label}... 0%`;
  const response = await fetch(`/partition?label=${label}`);
  if (!response.ok) throw new Error(`Failed to fetch ${label} (HTTP ${response.status})`);
  const contentLength = response.headers.get('content-length');
  const total = contentLength ? parseInt(contentLength, 10) : 0;
  const reader = response.body.getReader();
  const chunks = [];
  let loaded = 0;
  while (true) {
    const { done, value } = await reader.read();
    if (done) break;
    chunks.push(value);
    loaded += value.length;
    if (total && btn) btn.innerText = `Downloading ${label}... ${Math.round((loaded / total) * 100)}%`;
  }
  const blob = new Blob(chunks, { type: 'application/octet-stream' });
  const downloadUrl = window.URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.style.display = 'none';
  a.href = downloadUrl;
  a.download = `${window.location.hostname.replace('.local', '')}_${label}_backup.bin`;
  document.body.appendChild(a);
  a.click();
  window.URL.revokeObjectURL(downloadUrl);
  a.remove();
  return new Promise(resolve => setTimeout(resolve, 300));
}

async function backupAndRedirectToOTA(buttonId) {
  const btn = document.getElementById(buttonId);
  const originalText = btn ? btn.innerText : "";
  if (btn) btn.disabled = true;
  try {
    await downloadPartition("nvs", btn);
    await downloadPartition("spiffs", btn);
    await downloadPartition("zb_storage", btn);
    if (btn) btn.innerText = "Redirecting to OTA...";
    setTimeout(() => window.location.assign("/update"), 500);
  } catch (error) {
    alert(`Backup failed: ${error.message}. Redirecting to OTA anyway...`);
    window.location.assign("/update");
    if (btn) { btn.innerText = originalText; btn.disabled = false; }
  }
}

async function backupAndContinue(buttonId) {
  const btn = document.getElementById(buttonId);
  const originalText = btn ? btn.innerText : "";
  if (btn) btn.disabled = true;
  await downloadPartition("nvs", btn);
  await downloadPartition("spiffs", btn);
  await downloadPartition("zb_storage", btn);
  if (btn) { btn.innerText = originalText; btn.disabled = false; }
  const numericId = buttonId.replace("btn", "");
  websock.send("bdown:" + numericId);
  setTimeout(() => websock.send("bup:" + numericId), 50);
}

const originalButtonClick = window.buttonclick;
window.buttonclick = function(number, isdown) {
  if (number == 34) { if (!isdown) backupAndContinue("btn34"); return; }
  if (number == 30) { if (!isdown) backupAndRedirectToOTA("btn30"); return; }
  if (number == 31) { if (!isdown) redirect2ZigbeeOTA(); return; }
  originalButtonClick(number, isdown);
};

function updateActionThresholdsVisibility() {
  let eventSelect = null;
  for (const sel of document.querySelectorAll("select")) {
    if (sel.options.length > 0 && sel.options[0].text.includes("Select source event")) {
      eventSelect = sel;
      break;
    }
  }
  if (!eventSelect) return;
  const panel = eventSelect.closest(".panel") || eventSelect.parentElement.parentElement;
  if (!panel) return;
  let thresh1Wrap = null, thresh2Wrap = null;
  panel.querySelectorAll("input[type='text']").forEach(inp => {
    const wrap = inp.parentElement;
    if (!wrap) return;
    const lt = wrap.textContent.toLowerCase();
    if (lt.includes("main threshold")) thresh1Wrap = wrap;
    else if (lt.includes("secondary threshold")) thresh2Wrap = wrap;
  });
  if (!thresh1Wrap || !thresh2Wrap) return;
  const val = parseInt(eventSelect.value);
  const selectedText = eventSelect.options[eventSelect.selectedIndex] ? eventSelect.options[eventSelect.selectedIndex].text.toLowerCase() : "";
  const isCondition = !isNaN(val) && val >= 0xFFFF;
  thresh1Wrap.style.display = isCondition ? "" : "none";
  thresh2Wrap.style.display = (isCondition && selectedText.includes("between")) ? "" : "none";
}

function syncSubactionDropdown() {
  if (!document.querySelector("select")) return;
  let subInput = document.getElementById("text" + (typeof action_subaction_number !== 'undefined' ? action_subaction_number : 9999)) ||
                 document.getElementById("num" + (typeof action_subaction_number !== 'undefined' ? action_subaction_number : 9999));
  if (!subInput) {
    document.querySelectorAll("input").forEach(inp => {
      const wrap = inp.closest(".panel") || inp.parentElement;
      if (wrap && wrap.textContent.includes("Subaction ID")) subInput = inp;
    });
  }
  if (!subInput) return;
  let actionSelect = null;
  for (const sel of document.querySelectorAll("select")) {
    if (sel.options.length > 0 && sel.options[0].text.includes("Select destination action")) {
      actionSelect = sel;
      break;
    }
  }
  if (!actionSelect) return;
  if (!document.getElementById("z2s-subaction-css")) {
    const st = document.createElement("style");
    st.id = "z2s-subaction-css";
    st.textContent = `.z2s-force-hidden{display:none!important}.subaction-helper-select,.subaction-helper-select option{width:100%!important;box-sizing:border-box!important;color:#212529!important;background-color:#fff!important}.subaction-helper-select:disabled{opacity:.5!important;pointer-events:none!important}`;
    document.head.appendChild(st);
  }
  const isFormDisabled = !window.z2sActionEditActive || subInput.disabled || actionSelect.disabled;
  const selectedActionText = (!isFormDisabled && actionSelect.selectedIndex >= 0 && actionSelect.options[actionSelect.selectedIndex])
    ? actionSelect.options[actionSelect.selectedIndex].text.toUpperCase() : "";
  const isIrMode = selectedActionText.includes("SEND IR CODE");
  const isPoMode = selectedActionText.includes("SEND PUSHOVER MESSAGE");

  let helperSelect = subInput.parentElement.querySelector(".subaction-helper-select");
  if (!helperSelect) {
    helperSelect = document.createElement("select");
    helperSelect.className = (actionSelect.className + " subaction-helper-select z2s-force-hidden").trim();
    subInput.insertAdjacentElement("afterend", helperSelect);
    helperSelect.addEventListener("change", function() {
      subInput.value = this.value;
      const numId = subInput.id.split("num").join("").split("text").join("");
      if (typeof numberchange === "function" && numId) numberchange(numId);
      else subInput.dispatchEvent(new Event("change", { bubbles: true }));
    });
  }
  if (isFormDisabled || (!isIrMode && !isPoMode)) {
    helperSelect.value = "0";
    helperSelect.disabled = true;
    helperSelect.classList.add("z2s-force-hidden");
    subInput.classList.remove("z2s-force-hidden");
    return;
  }
  const optionsData = isIrMode ? (window.z2sIrOptions || []) : (window.z2sPoOptions || []);
  const placeholder = isIrMode ? "Select IR code (sid)..." : "Select Pushover message (sid)...";
  const currentVal = String(subInput.value || "0");
  let html = `<option value="0">${placeholder}</option>`;
  if (optionsData.length === 0) {
    html += `<option value="${currentVal}" selected>No saved items (sid: ${currentVal})</option>`;
  } else {
    optionsData.forEach(opt => {
      const selected = String(opt.id) === currentVal ? " selected" : "";
      html += `<option value="${opt.id}"${selected}>${opt.label.split('"').join("&quot;")}</option>`;
    });
  }
  helperSelect.innerHTML = html;
  helperSelect.disabled = false;
  subInput.classList.add("z2s-force-hidden");
  helperSelect.classList.remove("z2s-force-hidden");
}

document.addEventListener("change", function(e) {
  if (e.target && e.target.tagName === "SELECT" && !e.target.classList.contains("subaction-helper-select")) {
    updateActionThresholdsVisibility();
    syncSubactionDropdown();
    setTimeout(syncSubactionDropdown, 150);
    setTimeout(syncSubactionDropdown, 350);
  }
});

const origUpdateSelectOptions = updateSelectOptions;
updateSelectOptions = function(...args) {
  origUpdateSelectOptions.apply(this, args);
  setTimeout(() => {
    updateActionThresholdsVisibility();
    syncSubactionDropdown();
  }, 50);
};

window.addEventListener("beforeunload", function() {
  if (typeof websock !== "undefined" && websock && websock.readyState === WebSocket.OPEN) websock.close();
});

window.addEventListener("load", function() {
  setTimeout(function checkSync() {
    const anySelect = document.querySelector("select");
    if (anySelect && anySelect.options.length === 0) {
      if (typeof websock !== "undefined" && websock && websock.readyState === WebSocket.OPEN) websock.send("uicc:1");
    }
  }, 1200);
});