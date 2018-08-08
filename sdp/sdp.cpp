#include "sdp.h"

#include <regex>

std::string Sdp::CreateOffer()
{
  return "";
}

std::string Sdp::CreateAnswer(const std::string& remote_sdp)
{
  this->sdp_parse(remote_sdp);
  this->sdp_generate_answer();
  this->sdp_generate_answer_1();
  return this->sdp_write();
}

std::vector<std::string> Sdp::StrSplit(const std::string& str, const std::string& s)
{
  std::vector<std::string> ret;

  std::regex pat(s);
  std::sregex_token_iterator iter(str.begin(),
    str.end(),
    pat,
    -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    ret.push_back(*iter);
  //std::cout << *iter << '\n';
  return ret;
}


void Sdp::sdp_parse(const std::string& sdp)
{
  offer_sdp_ = std::make_unique<SdpObj>();

  if (sdp.substr(0, 2) != "v=") {
    // error
  }

  std::vector<std::string> parts = this->StrSplit(sdp, "\r\n");

  bool is_mline = false;
  for (int i = 0; i < parts.size(); i) {
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
        this->offer_sdp_->version = atoi(line.substr(2).c_str());
        break;
      }
      case 'o': {
        std::vector<std::string> v = this->StrSplit(line, " ");
        this->offer_sdp_->o_name = v[0].substr(2);
        this->offer_sdp_->o_sessid = atoll(v[1].c_str());
        this->offer_sdp_->o_version = atol(v[2].c_str());
        this->offer_sdp_->o_addr = v[5];
        if (v[4] == "IP4")
          this->offer_sdp_->o_ipv4 = true;
        else
          this->offer_sdp_->o_ipv4 = false;
        break;
      }
      case 's': {
        this->offer_sdp_->s_name = line.substr(2);
        break;
      }
      case 't': {
        std::vector<std::string> v = this->StrSplit(line, " ");
        this->offer_sdp_->t_start = atoll(v[0].substr(2).c_str());
        this->offer_sdp_->t_stop = atoll(v[1].c_str());
        break;
      }
      case 'a': {
        sdp_attribute a;
        auto i = line.find(':');
        if (i == std::string::npos) {
          a.name = line.substr(2);
        }
        else {
          a.name = line.substr(2, i - 2);
          a.value = line.substr(i + 1);
          a.direction = SDP_DEFAULT;
        }
        this->offer_sdp_->attributes.push_back(a);
        break;
      }
      case 'm': {
        sdp_mline m;
        std::vector<std::string> v = this->StrSplit(line, " ");
        m.type = this->sdp_parse_mtype(v[0].substr(2));
        m.type_str = v[0].substr(2);
        m.port = atoi(v[1].c_str());
        m.proto = v[2];
        if (m.port > 0) { // 否则表示禁用
          m.fmts = std::vector<std::string>(v.begin() + 3, v.end());
          for (auto const& i : m.fmts)
            m.ptypes.push_back(atoi(i.c_str()));
        }
        is_mline = true;
        this->offer_sdp_->m_lines.push_back(m);
        break;
      }
      default:
        //std::cout << "wrong\n";
        break;
      }
    }
    else {
      /* m= */
      switch (line[0]) {
      case 'c': {
        sdp_mline &mline = this->offer_sdp_->m_lines[this->offer_sdp_->m_lines.size() - 1];

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
        sdp_mline &mline = this->offer_sdp_->m_lines[this->offer_sdp_->m_lines.size() - 1];

        sdp_attribute a;
        auto i = line.find(':');
        if (i == std::string::npos) {
          sdp_mdirection direction = this->sdp_parse_mdirection(line.substr(2));
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
        //std::cout << "wrong\n";
        break;
      }
    }

    ++i;
  }
}

Sdp::sdp_mtype Sdp::sdp_parse_mtype(const std::string& type)
{
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

Sdp::sdp_mdirection Sdp::sdp_parse_mdirection(const std::string& direction) {
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


void Sdp::sdp_generate_answer()
{
  answer_sdp_ = std::make_unique<SdpObj>();
  answer_sdp_->version = offer_sdp_->version;
  answer_sdp_->o_name = offer_sdp_->o_name; // "-"
  answer_sdp_->o_sessid = offer_sdp_->o_sessid;
  answer_sdp_->o_version = offer_sdp_->o_version;
  answer_sdp_->o_ipv4 = offer_sdp_->o_ipv4;
  answer_sdp_->o_addr = offer_sdp_->o_addr; // "127.0.0.1"
  answer_sdp_->s_name = offer_sdp_->s_name; // "Janus session"
  answer_sdp_->t_start = 0;
  answer_sdp_->t_stop = 0;
  answer_sdp_->c_ipv4 = offer_sdp_->c_ipv4;
  answer_sdp_->c_addr = offer_sdp_->c_addr; // "127.0.0.1"

  const std::vector<sdp_mline> &m_lines = offer_sdp_->m_lines;
  for (int i = 0; i < m_lines.size(); ++i) {
    const sdp_mline &m = m_lines[i];
    sdp_mline am;
    am.type = m.type;
    am.type_str = m.type_str;
    am.port = m.port;
    am.proto = m.proto; // "UDP/TLS/RTP/SAVPF"
    am.c_ipv4 = m.c_ipv4;
    am.c_addr = am.c_addr; // "127.0.0.1"

    if (m.type == SDP_AUDIO) {

    }
    else if (m.type == SDP_VIDEO) {
      std::string codec = "vp8";
      int pt = 96;
      am.direction = SDP_SENDRECV;
      am.attributes.emplace_back("rtpmap", "96 VP8/90000", SDP_DEFAULT);
      am.attributes.emplace_back("rtcp-fb", "96 ccm fir", SDP_DEFAULT);
      am.attributes.emplace_back("rtcp-fb", "96 nack", SDP_DEFAULT);
      am.attributes.emplace_back("rtcp-fb", "96 nack pli", SDP_DEFAULT);
      am.attributes.emplace_back("rtcp-fb", "96 goog-remb", SDP_DEFAULT);
      am.attributes.emplace_back("rtcp-fb", "96 transport-cc", SDP_DEFAULT);
    }
    else {

    }
    answer_sdp_->m_lines.push_back(am);
  }
}

std::string Sdp::sdp_write()
{
  std::string sdp;
  char buffer[1024];
  int written = 0;

  /* v= */
  written = snprintf(buffer, sizeof(buffer), "v=%d\r\n", this->answer_sdp_->version);
  buffer[written] = '\0';
  sdp += buffer;
  /* o= */
  written = snprintf(buffer, sizeof(buffer), "o=%s %lld %lld IN %s %s\r\n",
    this->answer_sdp_->o_name.c_str(), this->answer_sdp_->o_sessid, this->answer_sdp_->o_version,
    this->answer_sdp_->o_ipv4 ? "IP4" : "IP6", this->answer_sdp_->o_addr.c_str());
  buffer[written] = '\0';
  sdp += buffer;
  /* s= */
  written = snprintf(buffer, sizeof(buffer), "s=%s\r\n", this->answer_sdp_->s_name.c_str());
  buffer[written] = '\0';
  sdp += buffer;
  /* t= */
  written = snprintf(buffer, sizeof(buffer), "t=%lld %lld\r\n", this->answer_sdp_->t_start, this->answer_sdp_->t_stop);
  buffer[written] = '\0';
  sdp += buffer;
  /* a= */
  // 全局a=，存在std::vector<sdp_attribute>中
  for (int i = 0; i < this->answer_sdp_->attributes.size(); ++i) {
    const sdp_attribute &a = this->answer_sdp_->attributes[i];
    if (a.value.size() != 0) {
      written = snprintf(buffer, sizeof(buffer), "a=%s:%s\r\n", a.name.c_str(), a.value.c_str());
    }
    else {
      written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", a.name.c_str());
    }
    buffer[written] = '\0';
    sdp += buffer;
  }
  /* m= */
  // 存在std::vector<sdp_mline>中
  for (int i = 0; i < this->answer_sdp_->m_lines.size(); ++i) {
    const sdp_mline &m = this->answer_sdp_->m_lines[i];
    // "m=video 9 UDP/TLS/RTP/SAVPF 96\r\n"
    written = snprintf(buffer, sizeof(buffer), "m=%s %d %s %d\r\n", m.type_str.c_str(), m.port, m.proto.c_str(), 96);
    buffer[written] = '\0';
    sdp += buffer;
    /* c= */
    if (m.c_addr.size() != 0) {
      written = snprintf(buffer, sizeof(buffer), "c=IN %s %s\r\n",
        m.c_ipv4 ? "IP4" : "IP6", m.c_addr.c_str());
      buffer[written] = '\0';
      sdp += buffer;
    }
    /* a= (note that we don't format the direction if it's JANUS_SDP_DEFAULT) */
    std::string direction = this->sdp_mdirection_str(m.direction);
    written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", direction.c_str());
    buffer[written] = '\0';
    sdp += buffer;
    /* 属于m=的a= */
    for (int j = 0; j < m.attributes.size(); ++j) {
      const sdp_attribute &a = m.attributes[j];
      if (a.value.size() != 0) {
        written = snprintf(buffer, sizeof(buffer), "a=%s:%s\r\n", a.name.c_str(), a.value.c_str());
      }
      else {
        written = snprintf(buffer, sizeof(buffer), "a=%s\r\n", a.name.c_str());
      }
      buffer[written] = '\0';
      sdp += buffer;
    }
  }
  return sdp;
}

std::string Sdp::sdp_mdirection_str(sdp_mdirection direction)
{
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

void Sdp::sdp_generate_answer_1()
{
  /* 全局 */
  // public ip
  this->answer_sdp_->o_addr; 

  // 不要全局的c=

  // a=bundle
  this->answer_sdp_->attributes.emplace_back("group", "BUNDLE video", SDP_DEFAULT);
  this->answer_sdp_->attributes.emplace_back("msid-semantic", " WMS janus", SDP_DEFAULT);

  /* 局部 */
  this->answer_sdp_->m_lines[0].c_addr = "127.0.0.1";

  // ice相关
  {
    std::vector<sdp_attribute> attributes;
    attributes.push_back(sdp_attribute("ice-ufrag", "xxxx", SDP_DEFAULT));
    attributes.push_back(sdp_attribute("ice-pwd", "xxxx", SDP_DEFAULT));
    attributes.push_back(sdp_attribute("ice-options", "xxxx", SDP_DEFAULT));
    attributes.push_back(sdp_attribute("fingerprint", "xxxx", SDP_DEFAULT));
    attributes.push_back(sdp_attribute("setup", "xxxx", SDP_DEFAULT));
    this->answer_sdp_->m_lines[0].attributes.insert(this->answer_sdp_->m_lines[0].attributes.begin(), attributes.begin(), attributes.end());
  }
  //
  {
    std::vector<sdp_attribute> attributes;
    attributes.push_back(sdp_attribute("mid", "video", SDP_DEFAULT));
    attributes.push_back(sdp_attribute("rtcp-mux", "", SDP_DEFAULT));
    this->answer_sdp_->m_lines[0].attributes.insert(this->answer_sdp_->m_lines[0].attributes.begin(), attributes.begin(), attributes.end());
  }
  // rtcp and ssrc(mid)
  this->answer_sdp_->m_lines[0].attributes.emplace_back("ssrc", "xxxx cname:janusvideo", SDP_DEFAULT);
  this->answer_sdp_->m_lines[0].attributes.emplace_back("ssrc", "xxxx msid:janus janusv0", SDP_DEFAULT);
  this->answer_sdp_->m_lines[0].attributes.emplace_back("ssrc", "xxxx mslabel:janus", SDP_DEFAULT);
  this->answer_sdp_->m_lines[0].attributes.emplace_back("ssrc", "xxxx label:janusv0", SDP_DEFAULT);

  // ice candidate
  this->answer_sdp_->m_lines[0].attributes.emplace_back("candidate", "xxxx", SDP_DEFAULT);
  this->answer_sdp_->m_lines[0].attributes.emplace_back("end-of-candidates", "", SDP_DEFAULT);
}