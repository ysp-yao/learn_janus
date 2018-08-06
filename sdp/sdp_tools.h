#include "ulti.h"

#include <string>
#include <vector>
#include <inttypes.h>

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
  std::string     name;       /*! \brief Attribute name */
  std::string     value;      /*! \brief Attribute value */
  sdp_mdirection  direction;  /*! \brief Attribute direction (e.g., for extmap) */
};


/*! \brief SDP m-line representation */
struct sdp_mline {
  sdp_mtype                 type;         /*! \brief Media type as a janus_sdp_mtype enumerator */
  std::string               type_str;     /*! \brief Media type (string) */
  uint16_t                  port;         /*! \brief Media port */
  std::string               proto;        /*! \brief Media protocol */
  std::vector<std::string>  fmts;         /*! \brief List of formats */
  std::vector<int>          ptypes;       /*! \brief List of payload types */
  bool                      c_ipv4;       /*! \brief Media c= protocol */
  std::string               c_addr;       /*! \brief Media c= address */
  std::string               b_name;       /*! \brief Media b= type */
  uint32_t                  b_value;      /*! \brief Media b= value */
  sdp_mdirection            direction;    /*! \brief Media direction */
  std::vector<std::string>  attributes;   /*! \brief List of m-line attributes */
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

sdp_mtype sdp_parse_mtype(const std::string& type) {
  if (type.size() == 0)
    return SDP_OTHER;
  if (type == "audio")
    return SDP_AUDIO;
  if (type == "video")
    return SDP_VIDEO;
  if (type == "application")
    return SDP_APPLICATION;
  return SDP_OTHER;
}


SdpObj SdpParse(const std::string& sdp)
{
  SdpObj imported;

  if (sdp.substr(0, 2) != "v=") {
    // error
  }
  
  imported.o_ipv4 = true;
  imported.c_ipv4 = true;

  std::vector<std::string> parts = StrSplit(sdp, "\r\n");
  //for (const auto& line : parts) {
  bool is_mline = false;
  for (int i=0; i < parts.size();i) {
    std::string line = parts[i];
    if (line.size() == 0) {
      ++i;
      continue;
    }
    if (line.size() < 3) {
      // error
      break;
    }
    if (line[1] != '=') {
      // error
      break;
    }

    if (!is_mline) {
      switch (line[0]) {
        case 'v': {
          sscanf(line.c_str(), "v=%d", &imported.version);
          break;
        }
        case 'o': {
          std::vector<std::string> v = StrSplit(line, " ");
          imported.o_name = v[0].substr(2);
          imported.o_sessid = atoll(v[1].c_str());
          imported.o_version = atol(v[2].c_str());
          imported.o_addr = v[5];
          if (v[4] == "IP4")
            imported.o_ipv4 = true;
          else 
            imported.o_ipv4 = false;
          break;
        }
        case 's': {
          imported.s_name = line.substr(2);
          break;
        }
        case 't': {
          std::vector<std::string> v = StrSplit(line, " ");
          imported.t_start = atoll(v[0].substr(2).c_str());
          imported.t_stop = atoll(v[1].c_str());
          break;
        }
        case 'a': {
          sdp_attribute a;
          auto i = line.find(':');
          if (i == std::string::npos) {
            a.name = line.substr(2);
          }
          else {
            a.name = line.substr(2, i-2);
            a.value = line.substr(i + 1);
            a.direction = SDP_DEFAULT;
          }
          imported.attributes.push_back(a);
          break;
        }
        case 'm': {
          sdp_mline m;
          std::vector<std::string> v = StrSplit(line, " ");

          m.type = sdp_parse_mtype(v[0].substr(2));
          m.type_str = v[0].substr(2);
          m.port = atoi(v[1].c_str());
          m.proto = v[2];
          m.direction = SDP_SENDRECV;
          if (m.port > 0) {
            m.fmts = std::vector<std::string>(v.begin()+3, v.end());
            for (auto const& i : m.fmts)
              m.ptypes.push_back(atoi(i.c_str()));
          }

          is_mline = true;
          imported.m_lines.push_back(m);
          break;
        }
        default:
          std::cout << "wrong\n";
          break;
      }
    }
    else {
      switch (line[0]) {
        sdp_mline mline = imported.m_lines[imported.m_lines.size()-1];
        case 'c': {
          std::vector<std::string> v = StrSplit(line, " ");

          if (v[1] == "IP4") {
            mline.c_ipv4 = true;
          }
          else if (v[1] == "IP6") {
            mline.c_ipv4 = false;
          }
          else {
            // error
          }
          mline.c_addr = v[2];

          break;
        }
        case 'a': {
          sdp_attribute a;
          auto i = line.find(':');
          if (i == std::string::npos) {
            /* Is this a media direction attribute? */
            mline.direction;
          }
          else {
            a.name = line.substr(2, i - 2);
            a.value = line.substr(i + 1);
            a.direction = SDP_DEFAULT;
          }
          imported.attributes.push_back(a);
          break;
        }
        case 'm': {
          is_mline = false;
          continue;
        }
        default:
          std::cout << "wrong\n";
          break;
      }
    }

    ++i;
  }

  return imported;
}