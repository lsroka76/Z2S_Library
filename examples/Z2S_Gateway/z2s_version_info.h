#ifndef Z2S_VERSION_INFO_H_
#define Z2S_VERSION_INFO_H_

#define Z2S_VERSION "0.8.91-13/08/2025"

#ifdef USE_SUPLA_WEB_SERVER 

namespace Supla {
namespace Html {

class CustomDevInfo : public Supla::HtmlElement {
 public:  
  CustomDevInfo() : HtmlElement(HTML_SECTION_DEVICE_INFO) {}

  void send(Supla::WebSender* sender) {
    sender->send("<span>");
    sender->send("<br>Z2S gateway ver.: ");
    sender->send(Z2S_VERSION);
    sender->send("<br>Flash chip real size::");
    sender->send(ESP.getFlashChipSize());
    sender->send(", Free sketch size:");
    sender->send(ESP.getFreeSketchSpace());
    sender->send("<br>FreeHeap:");
    sender->send(ESP.getFreeHeap());
    sender->send(", MinFreeHeap:");
    sender->send(ESP.getMinFreeHeap());
    sender->send(", HeapSize:");
    sender->send(ESP.getHeapSize());
    sender->send(", MaxAlloc:");
    sender->send(ESP.getMaxAllocHeap());
    sender->send("</span>");
  }

}; // CustomDevInfo

}; // namespace Html
}; // namespace Supla
#endif //USE_SUPLA_WEB_SERVER
#endif
