#ifndef Z2S_VERSION_INFO_H_
#define Z2S_VERSION_INFO_H_

#define Z2S_VERSION "0.4.1-28/02/2025"

namespace Supla {
namespace Html {

class CustomDevInfo : public Supla::HtmlElement {
 public:  
  CustomDevInfo() : HtmlElement(HTML_SECTION_DEVICE_INFO) {}

  void send(Supla::WebSender* sender) {
    sender->send("<span>");
    sender->send("<br>Board rev.: ");
    sender->send(Z2S_VERSION);
    sender->send("<br>Flash chip real size: ");
    sender->send(ESP.getFlashChipSize());
    sender->send(", Free sketch size ");
    sender->send(ESP.getFreeSketchSpace());
    sender->send("</span>");
  }

}; // CustomDevInfo

}; // namespace Html
}; // namespace Supla

#endif