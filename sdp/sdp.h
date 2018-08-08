#ifndef SDP_H_
#define SDP_H_

#include <vector>
#include <string>
#include <memory>

class Sdp {
  /*! \brief Helper enumeration to quickly identify m-line directions */
  enum sdp_mdirection {
    SDP_DEFAULT,  /*! \brief default=sendrecv */
    SDP_SENDRECV,  /*! \brief sendrecv */
    SDP_SENDONLY,  /*! \brief sendonly */
    SDP_RECVONLY,  /*! \brief recvonly */
    SDP_INACTIVE,  /*! \brief inactive */
    SDP_INVALID  /*! \brief invalid direction (when parsing) */
  };

  enum sdp_mtype {
    SDP_AUDIO,        /*! \brief m=audio */
    SDP_VIDEO,        /*! \brief m=video */
    SDP_APPLICATION,  /*! \brief m=application */
    SDP_OTHER         /*! \brief m=whatever (we don't care, unsupported) */
  };

  /*! \brief SDP a= attribute representation */
  struct sdp_attribute {
    sdp_attribute(){}
    sdp_attribute(std::string n, std::string v, sdp_mdirection d):name(n), value(v), direction(d){}
    std::string     name;       /*! \brief Attribute name */
    std::string     value;      /*! \brief Attribute value */
    sdp_mdirection  direction;  /*! \brief Attribute direction (e.g., for extmap) */
  };


  /*! \brief SDP m-line representation */
  struct sdp_mline {
    sdp_mtype                   type;         /*! \brief Media type as a janus_sdp_mtype enumerator */
    std::string                 type_str;     /*! \brief Media type (string) */
    uint16_t                    port;         /*! \brief Media port */
    std::string                 proto;        /*! \brief Media protocol */
    bool                        c_ipv4;       /*! \brief Media c= protocol */
    std::string                 c_addr;       /*! \brief Media c= address */
    std::vector<std::string>    fmts;         /*! \brief List of formats */
    std::vector<int>            ptypes;       /*! \brief List of payload types */
    sdp_mdirection              direction;    /*! \brief Media direction */
    std::vector<sdp_attribute>  attributes;   /*! \brief List of m-line attributes */
  };

  struct SdpObj {
    int                        version;    /*! \brief v= */
    std::string                o_name;     /*! \brief o= name */
    uint64_t                   o_sessid;   /*! \brief o= session ID */
    uint64_t                   o_version;  /*! \brief o= version */
    bool                       o_ipv4;     /*! \brief o= protocol */
    std::string                o_addr;     /*! \brief o= address */
    std::string                s_name;     /*! \brief s= */
    uint64_t                   t_start;    /*! \brief t= start */
    uint64_t                   t_stop;     /*! \brief t= stop */
    bool                       c_ipv4;      /*! \brief c= protocol (not rendered for WebRTC usage) */
    std::string                c_addr;      /*! \brief c= address (not rendered for WebRTC usage) */
    std::vector<sdp_attribute> attributes;  /*! \brief List of global a= attributes */
    std::vector<sdp_mline>     m_lines;     /*! \brief List of m= m-lines */
  };
public:
  std::string CreateOffer();

  std::string CreateAnswer(const std::string& remote_sdp);

private:
  std::vector<std::string> StrSplit(const std::string& str, const std::string& s);

private:

  std::string sdp_mdirection_str(sdp_mdirection direction);

  void sdp_parse(const std::string& sdp);

  sdp_mtype sdp_parse_mtype(const std::string& type);

  sdp_mdirection sdp_parse_mdirection(const std::string& direction);

  void sdp_generate_answer();

  void sdp_generate_answer_1();

  std::string sdp_write();

private:
  std::unique_ptr<SdpObj> offer_sdp_;
  std::unique_ptr<SdpObj> answer_sdp_;
  bool video_rtcpfb_ = true;

  uint32_t video_ssrc_; // 32Î»Ëæ»úÊý
  std::string cname_;
  std::string stream_id_;
  std::string track_id_;
};

#endif // SDP_H_