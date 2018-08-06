#include "sdp_tools.h"


#include <string>
#include <regex>
#include <iostream>
std::string sdp = 
"v=0\r\n"
"o=mozilla...THIS_IS_SDPARTA-61.0.1 5636015926933475144 0 IN IP4 0.0.0.0\r\n"
"s=-\r\n"
"t=0 0\r\n"
"a=fingerprint:sha-256 AC:32:40:CB:BC:B5:AD:09:CF:02:71:1F:86:02:E5:8F:74:BE:B6:50:BD:1A:0C:6B:19:AB:5C:46:ED:3E:AA:25\r\n"
"a=group:BUNDLE sdparta_0 sdparta_1\r\n"
"a=ice-options:trickle\r\n"
"a=msid-semantic:WMS *\r\n"
"m=video 9 UDP/TLS/RTP/SAVPF 120 121 126 97\r\n"
"c=IN IP4 0.0.0.0\r\n"
"a=sendrecv\r\n"
"a=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\n"
"a=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n"
"a=extmap:5 urn:ietf:params:rtp-hdrext:toffset\r\n"
"a=fmtp:126 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\n"
"a=fmtp:97 profile-level-id=42e01f;level-asymmetry-allowed=1\r\n"
"a=fmtp:120 max-fs=12288;max-fr=60\r\n"
"a=fmtp:121 max-fs=12288;max-fr=60\r\n"
"a=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\n"
"a=ice-ufrag:0b21e0bd\r\na=mid:sdparta_0\r\n"
"a=msid:{ddeadef2-0208-4157-b526-a0548f33a518} {2d75766a-6efd-4305-84df-fb6087874dc9}\r\n"
"a=rtcp-fb:120 nack\r\n"
"a=rtcp-fb:120 nack pli\r\n"
"a=rtcp-fb:120 ccm fir\r\n"
"a=rtcp-fb:120 goog-remb\r\n"
"a=rtcp-fb:121 nack\r\n"
"a=rtcp-fb:121 nack pli\r\n"
"a=rtcp-fb:121 ccm fir\r\n"
"a=rtcp-fb:121 goog-remb\r\n"
"a=rtcp-fb:126 nack\r\n"
"a=rtcp-fb:126 nack pli\r\n"
"a=rtcp-fb:126 ccm fir\r\n"
"a=rtcp-fb:126 goog-remb\r\n"
"a=rtcp-fb:97 nack\r\n"
"a=rtcp-fb:97 nack pli\r\n"
"a=rtcp-fb:97 ccm fir\r\n"
"a=rtcp-fb:97 goog-remb\r\n"
"a=rtcp-mux\r\n"
"a=rtpmap:120 VP8/90000\r\n"
"a=rtpmap:121 VP9/90000\r\n"
"a=rtpmap:126 H264/90000\r\n"
"a=rtpmap:97 H264/90000\r\n"
"a=setup:actpass\r\n"
"a=ssrc:761832234 cname:{afd4e55e-5a85-4639-be7d-2bcfbcc350ac}\r\n"
"m=application 9 DTLS/SCTP 5000\r\n"
"c=IN IP4 0.0.0.0\r\na=sendrecv\r\n"
"a=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\n"
"a=ice-ufrag:0b21e0bd\r\n"
"a=mid:sdparta_1\r\n"
"a=sctpmap:5000 webrtc-datachannel 256\r\n"
"a=setup:actpass\r\n"
"a=max-message-size:1073741823\r\n"
;








int main() {

  //std::vector<std::string> vec = StrSplit(sdp, "\r\n");

  SdpParse(sdp);

  return 0;
}