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

sdp_mdirection sdp_parse_mdirection(const std::string& direction) {
  if (direction.size() == 0)
    return SDP_INVALID;
  if (direction == "sendrecv")
    return SDP_SENDRECV;
  if (direction == "sendonly")
    return SDP_SENDONLY;
  if (direction == "recvonly")
    return SDP_RECVONLY;
  if (direction == "inactive")
    return SDP_INACTIVE;
  return SDP_INVALID;
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
    const std::string& line = parts[i];
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
      /* 全局 */
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
          if (m.port > 0) { // 否则表示禁用
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
      /* m= */
      switch (line[0]) {
        case 'c': {
          sdp_mline &mline = imported.m_lines[imported.m_lines.size() - 1];

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
          sdp_mline &mline = imported.m_lines[imported.m_lines.size() - 1];

          sdp_attribute a;
          auto i = line.find(':');
          if (i == std::string::npos) {
            /* Is this a media direction attribute? */
            sdp_mdirection direction = sdp_parse_mdirection(line.substr(2));
            if (direction != SDP_INVALID)
              mline.direction = direction;
            a.name = line.substr(2);
          }
          else {
            a.name = line.substr(2, i - 2);
            a.value = line.substr(i + 1);
            a.direction = SDP_DEFAULT;
          }
          mline.attributes.push_back(a);
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

int sdp_get_codec_pt(const SdpObj& sdp, const std::string& codec) {
  if (codec == "VP8")
    return 120;
  else // opus
    return -1;
}

std::string sdp_get_codec_rtpmap(const std::string& codec) {
  if (codec == "OPUS")
    return "opus/48000/2";
  else if (codec == "VP8")
    return "VP8/90000";
  else
    return "";
}



SdpObj SdpGenerateAnswer(SdpObj offer, ...) {

  bool do_audio = true, do_video = true, do_data = true,
    audio_dtmf = false, video_rtcpfb = true, h264_fmtp = true;
  const char *audio_codec = NULL, *video_codec = NULL;
  sdp_mdirection audio_dir = SDP_SENDRECV, video_dir = SDP_SENDRECV;

  SdpObj answer;
  /* Start by copying some of the headers */
  answer.version = offer.version;
  answer.o_name = offer.o_name; // "-"
  answer.o_sessid = offer.o_sessid;
  answer.o_version = offer.o_version;
  answer.o_ipv4 = offer.o_ipv4;
  answer.o_addr = offer.o_addr; // "127.0.0.1"
  answer.s_name = offer.s_name; // "Janus session"
  answer.t_start = 0;
  answer.t_stop = 0;
  answer.c_ipv4 = offer.c_ipv4;
  answer.c_addr = offer.c_addr; // "127.0.0.1"
  answer.attributes;
  answer.m_lines;

  /* Now iterate on all media, and let's see what we should do */
  int audio = 0, video = 0, data = 0;
  const std::vector<sdp_mline> &m_lines = offer.m_lines;

  for (int i = 0; i < m_lines.size(); ++i) {
    const sdp_mline &m = m_lines[i];
    sdp_mline am;
    am.type = m.type;
    am.type_str = m.type_str;
    am.proto = m.proto; // "UDP/TLS/RTP/SAVPF"
    am.port = m.port;
    am.c_ipv4 = m.c_ipv4;
    am.c_addr = am.c_addr; // "127.0.0.1"
    am.direction = SDP_INACTIVE;	/* We'll change this later */

    answer.m_lines.push_back(am);

    if (m.type == SDP_AUDIO) {
      if (m.port > 0) {
        audio++;
      }
      if (!do_audio || audio > 1) {
        /* Reject */
        am.port = 0;
        continue;
      }
    }
    else if (m.type == SDP_VIDEO && m.port > 0) {
      if (m.port > 0) {
        video++;
      }
      if (!do_video || video > 1) {
        /* Reject */
        am.port = 0;
        continue;
      }
    }
    else if (m.type == SDP_APPLICATION && m.port > 0) {
      if (m.port > 0) {
        data++;
      }
      if (!do_data || data > 1) {
        /* Reject */
        am.port = 0;
        continue;
      }
    }
    
    if (m.type == SDP_AUDIO || m.type == SDP_VIDEO) {
      sdp_mdirection target_dir = m.type==SDP_AUDIO ? audio_dir : video_dir;
      switch (m.direction) {
        case SDP_RECVONLY:
          am.direction = SDP_SENDONLY;
          break;
        case SDP_SENDONLY:
          am.direction = SDP_RECVONLY;
          break;
        case SDP_INACTIVE:
          /* Peer inactive, set inactive in the answer to */
          am.direction = SDP_INACTIVE;
          break;
        case SDP_SENDRECV:
        default:
          /* The peer is fine with everything, so use our constraint */
          am.direction = target_dir;
          break;
      }
      //std::string codec = "vp8";
      //int pt = 120;
      ///* Add rtpmap attribute */
      //janus_sdp_attribute *a = janus_sdp_attribute_create("rtpmap", "%d %s", pt, janus_sdp_get_codec_rtpmap(codec));
      //am->attributes = g_list_append(am->attributes, a);
      //if (video_rtcpfb) {
      //  /* Add rtcp-fb attributes */
      //  a = janus_sdp_attribute_create("rtcp-fb", "%d ccm fir", pt);
      //  am->attributes = g_list_append(am->attributes, a);
      //  a = janus_sdp_attribute_create("rtcp-fb", "%d nack", pt);
      //  am->attributes = g_list_append(am->attributes, a);
      //  a = janus_sdp_attribute_create("rtcp-fb", "%d nack pli", pt);
      //  am->attributes = g_list_append(am->attributes, a);
      //  a = janus_sdp_attribute_create("rtcp-fb", "%d goog-remb", pt);
      //  am->attributes = g_list_append(am->attributes, a);
      //  /* It is safe to add transport-wide rtcp feedback mesage here, won't be used unless the header extension is negotiated*/
      //  a = janus_sdp_attribute_create("rtcp-fb", "%d transport-cc", pt);
      //  am->attributes = g_list_append(am->attributes, a);
      //}
    }
    else { // datachannel

    }
  }

  return answer;
}

#define SDP_APPEND_LINE \
{ \
  buffer[written] = '\0'; \
  sdp += buffer; \
}

std::string sdp_mdirection_str(sdp_mdirection direction) {
  switch (direction) {
  case SDP_DEFAULT:
  case SDP_SENDRECV:
    return "sendrecv";
  case SDP_SENDONLY:
    return "sendonly";
  case SDP_RECVONLY:
    return "recvonly";
  case SDP_INACTIVE:
    return "inactive";
  case SDP_INVALID:
  default:
    break;
  }
  return NULL;
}


std::string SdpWrite(SdpObj imported) {

  std::string sdp;
  char buffer[1024];
  int written = 0;

  /* v= */
  written = snprintf(buffer, sizeof(buffer), "v=%d\r\n", imported.version);
  SDP_APPEND_LINE
  /* o= */
  written = snprintf(buffer, sizeof(buffer), "o=%s %lld %lld IN %s %s\r\n",
    imported.o_name.c_str(), imported.o_sessid, imported.o_version,
    imported.o_ipv4 ? "IP4" : "IP6", imported.o_addr.c_str());
  SDP_APPEND_LINE
  /* s= */
  written = snprintf(buffer, sizeof(buffer), "s=%s\r\n", imported.s_name.c_str());
  SDP_APPEND_LINE
  /* t= */
  written = snprintf(buffer, sizeof(buffer), "t=%lld lld\r\n", imported.t_start, imported.t_stop);
  SDP_APPEND_LINE
  /* c= */
  if (imported.c_addr.size() != 0) {
    written = snprintf(buffer, sizeof(buffer), "c=IN %s %s\r\n",
      imported.c_ipv4 ? "IP4" : "IP6", imported.c_addr.c_str());
    SDP_APPEND_LINE
  }
  /* a= */
  // 全局a=，存在std::vector<sdp_attribute>中
  for (int i = 0; i < imported.attributes.size(); ++i) {
    const sdp_attribute &a = imported.attributes[i];
    if (a.value.size() != 0) {
      written = snprintf(buffer, sizeof(buffer), "a=%s:%s\r\n", a.name.c_str(), a.value.c_str());
    }
    else {
      written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", a.name.c_str());
    }
    SDP_APPEND_LINE
  }
  /* m= */
  // 存在std::vector<sdp_mline>中
  for (int i = 0; i < imported.m_lines.size(); ++i) {
    const sdp_mline &m = imported.m_lines[i];
    written = snprintf(buffer, sizeof(buffer), "m=%s %d %s", m.type_str.c_str(), m.port, m.proto.c_str());
    SDP_APPEND_LINE
    if (m.port == 0) {
      /* Remove all payload types/formats if we're rejecting the media */
      sdp += " 0";
    }
    else {
      if (m.proto.find("RTP") != std::string::npos) {
        /* RTP profile, use payload types */

      }
      else {
        /* Something else, use formats */

      }
    }
    sdp += "\r\n";
    /* c= */
    if (m.c_addr.size() != 0) {
      written = snprintf(buffer, sizeof(buffer), "c=IN %s %s\r\n",
        m.c_ipv4 ? "IP4" : "IP6", m.c_addr.c_str());
      SDP_APPEND_LINE
    }
    /* a= (note that we don't format the direction if it's JANUS_SDP_DEFAULT) */
    std::string direction = sdp_mdirection_str(m.direction);
    written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", direction.c_str());
    SDP_APPEND_LINE
      if (m.port == 0) {
        /* No point going on */
        continue;
      }
    /* 属于m=的a= */
    for (int j = 0; j < m.attributes.size(); ++j) {
      const sdp_attribute &a = m.attributes[j];
      if (a.value.size() != 0) {
        written = snprintf(buffer, sizeof(buffer), "a=%s:%s\r\n", a.name.c_str(), a.value.c_str());
      }
      else {
        written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", a.name.c_str());
      }
      SDP_APPEND_LINE
    }
  }
  return sdp;
}
