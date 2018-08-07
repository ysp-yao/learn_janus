// v=0\r\n
// o=mozilla...THIS_IS_SDPARTA-61.0.1 5636015926933475144 0 IN IP4 0.0.0.0\r\n
// s=-\r\n
// t=0 0\r\n
// a=fingerprint:sha-256 AC:32:40:CB:BC:B5:AD:09:CF:02:71:1F:86:02:E5:8F:74:BE:B6:50:BD:1A:0C:6B:19:AB:5C:46:ED:3E:AA:25\r\n
// a=group:BUNDLE sdparta_0 sdparta_1\r\n
// a=ice-options:trickle\r\n
// a=msid-semantic:WMS *\r\n
// m=video 9 UDP/TLS/RTP/SAVPF 120 121 126 97\r\n
// c=IN IP4 0.0.0.0\r\na=sendrecv\r\n
// a=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\n
// a=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n
// a=extmap:5 urn:ietf:params:rtp-hdrext:toffset\r\n
// a=fmtp:126 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\n
// a=fmtp:97 profile-level-id=42e01f;level-asymmetry-allowed=1\r\n
// a=fmtp:120 max-fs=12288;max-fr=60\r\n
// a=fmtp:121 max-fs=12288;max-fr=60\r\n
// a=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\n
// a=ice-ufrag:0b21e0bd\r\na=mid:sdparta_0\r\n
// a=msid:{ddeadef2-0208-4157-b526-a0548f33a518} {2d75766a-6efd-4305-84df-fb6087874dc9}\r\n
// a=rtcp-fb:120 nack\r\n
// a=rtcp-fb:120 nack pli\r\n
// a=rtcp-fb:120 ccm fir\r\n
// a=rtcp-fb:120 goog-remb\r\n
// a=rtcp-fb:121 nack\r\n
// a=rtcp-fb:121 nack pli\r\n
// a=rtcp-fb:121 ccm fir\r\n
// a=rtcp-fb:121 goog-remb\r\n
// a=rtcp-fb:126 nack\r\n
// a=rtcp-fb:126 nack pli\r\n
// a=rtcp-fb:126 ccm fir\r\n
// a=rtcp-fb:126 goog-remb\r\n
// a=rtcp-fb:97 nack\r\n
// a=rtcp-fb:97 nack pli\r\n
// a=rtcp-fb:97 ccm fir\r\n
// a=rtcp-fb:97 goog-remb\r\n
// a=rtcp-mux\r\n
// a=rtpmap:120 VP8/90000\r\n
// a=rtpmap:121 VP9/90000\r\n
// a=rtpmap:126 H264/90000\r\n
// a=rtpmap:97 H264/90000\r\n
// a=setup:actpass\r\n
// a=ssrc:761832234 cname:{afd4e55e-5a85-4639-be7d-2bcfbcc350ac}\r\n
// m=application 9 DTLS/SCTP 5000\r\n
// c=IN IP4 0.0.0.0\r\na=sendrecv\r\n
// a=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\n
// a=ice-ufrag:0b21e0bd\r\n
// a=mid:sdparta_1\r\n
// a=sctpmap:5000 webrtc-datachannel 256\r\n
// a=setup:actpass\r\n
// a=max-message-size:1073741823\r\n"






var localStream = null;
var local_sdp = null;
var ws = null;
var session_id = null;
var handle_id = null;
var pc1 = null;
var ice = null;

function keepalive() {
  var json = {
    janus: "keepalive",
    session_id: session_id,
    transaction: "6VjITvGtDWs2"
  };
  console.log("<--", json);
  var json_str = JSON.stringify(json);
  ws.send(json_str);

}


var localVideo = document.getElementById('localVideo');
localVideo.addEventListener('loadedmetadata', function() {
  console.log('Local video videoWidth: ' + this.videoWidth +
  'px,  videoHeight: ' + this.videoHeight + 'px');
});

var remoteVideo = document.getElementById('remoteVideo');
remoteVideo.addEventListener('loadedmetadata', function() {
  console.log('Local video videoWidth: ' + this.videoWidth +
  'px,  videoHeight: ' + this.videoHeight + 'px');
});

remoteVideo.onresize = function() {
  console.log("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
};

function Connect() {
  //ws = new WebSocket('ws://192.168.1.118:8188', 'janus-protocol');
  ws = new WebSocket('ws://172.16.16.194:8188', 'janus-protocol');
  
  ws.onopen = function()
  {
    var json = {};
    json.janus = "create";
    json.transaction = "12345678";
  
    console.log("<--", json);
    var jsonStr = JSON.stringify(json);
    ws.send(jsonStr);
  };
  var a = 0;
  ws.onmessage = function (evt) 
  { 
    var json_recv = JSON.parse(evt.data);
    console.log("-->", json_recv);
  
    if (a === 0) {
      a = 1;

      session_id = json_recv.data.id;
          
      var json = {};
      json.janus = "attach";
      json.opaque_id = "echotest-YWwuJiQLkEy5";
      json.plugin = "janus.plugin.echotest";
      json.transaction = "fhv8hnLNupvj";
      json.session_id = session_id;
      
      console.log("<--", json);
      var jsonStr = JSON.stringify(json);
      ws.send(jsonStr);
    }
    else if (a ===1) {
      a = 2;

      handle_id = json_recv.data.id;
          
      var json = {
        body : {
          audio: true,
          video :true
        },
        janus:"message",
        session_id:session_id,
        handle_id:handle_id,
        transaction:"wEbZCqCmK3Lm"
      };
  
      console.log("<--", json);
      var jsonStr = JSON.stringify(json);
      ws.send(jsonStr);
    }
    else if (a ===2) {
      a = 3;
          
      var json = {
        body : {
          audio: true,
          video :true
        },
        janus:"message",
        session_id:session_id,
        handle_id:handle_id,
        transaction:"asasdasdasdas",
        jsep:{
          type : "offer",
          //sdp: "v=0\r\no=mozilla...THIS_IS_SDPARTA-61.0.1 5636015926933475144 0 IN IP4 0.0.0.0\r\ns=-\r\nt=0 0\r\na=fingerprint:sha-256 AC:32:40:CB:BC:B5:AD:09:CF:02:71:1F:86:02:E5:8F:74:BE:B6:50:BD:1A:0C:6B:19:AB:5C:46:ED:3E:AA:25\r\na=group:BUNDLE sdparta_0 sdparta_1\r\na=ice-options:trickle\r\na=msid-semantic:WMS *\r\nm=video 9 UDP/TLS/RTP/SAVPF 120 121 126 97\r\nc=IN IP4 0.0.0.0\r\na=sendrecv\r\na=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:5 urn:ietf:params:rtp-hdrext:toffset\r\na=fmtp:126 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\na=fmtp:97 profile-level-id=42e01f;level-asymmetry-allowed=1\r\na=fmtp:120 max-fs=12288;max-fr=60\r\na=fmtp:121 max-fs=12288;max-fr=60\r\na=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\na=ice-ufrag:0b21e0bd\r\na=mid:sdparta_0\r\na=msid:{ddeadef2-0208-4157-b526-a0548f33a518} {2d75766a-6efd-4305-84df-fb6087874dc9}\r\na=rtcp-fb:120 nack\r\na=rtcp-fb:120 nack pli\r\na=rtcp-fb:120 ccm fir\r\na=rtcp-fb:120 goog-remb\r\na=rtcp-fb:121 nack\r\na=rtcp-fb:121 nack pli\r\na=rtcp-fb:121 ccm fir\r\na=rtcp-fb:121 goog-remb\r\na=rtcp-fb:126 nack\r\na=rtcp-fb:126 nack pli\r\na=rtcp-fb:126 ccm fir\r\na=rtcp-fb:126 goog-remb\r\na=rtcp-fb:97 nack\r\na=rtcp-fb:97 nack pli\r\na=rtcp-fb:97 ccm fir\r\na=rtcp-fb:97 goog-remb\r\na=rtcp-mux\r\na=rtpmap:120 VP8/90000\r\na=rtpmap:121 VP9/90000\r\na=rtpmap:126 H264/90000\r\na=rtpmap:97 H264/90000\r\na=setup:actpass\r\na=ssrc:761832234 cname:{afd4e55e-5a85-4639-be7d-2bcfbcc350ac}\r\nm=application 9 DTLS/SCTP 5000\r\nc=IN IP4 0.0.0.0\r\na=sendrecv\r\na=ice-pwd:7e13d0eecfc46b3eae59177676e3508a\r\na=ice-ufrag:0b21e0bd\r\na=mid:sdparta_1\r\na=sctpmap:5000 webrtc-datachannel 256\r\na=setup:actpass\r\na=max-message-size:1073741823\r\n"
          sdp : local_sdp
        }
      };
  
      console.log("<--", json);
      var jsonStr = JSON.stringify(json);
      ws.send(jsonStr);
  
    }
    else if (a === 3) {
      if (json_recv.janus && json_recv.jsep) {
        a === 4;
        console.log(json_recv.jsep);

        pc1.setRemoteDescription(json_recv.jsep).then(
          function() {
            console.log("setRemoteDescription succeed!");
          },
          //onSetSessionDescriptionError
          function() {
            console.log("setRemoteDescription failed!");
          }
        );

        var json = {
          candidate: {
            candidate:ice.candidate,
            sdpMLineIndex:ice.sdpMLineIndex,
            sdpMid:ice.sdpMid
          },
          handle_id: handle_id,
          janus: "trickle",
          session_id: session_id,
          transaction: "dG0jXeelTW0N"
        }
        console.log("<--", json);
        var json_str = JSON.stringify(json);
        ws.send(json_str);
  
        setInterval(keepalive, 30000);
      }      
    }
  
  };
   
  ws.onclose = function()
  { 
  
  };
  
  ws.onerror = function()
  {
    
  }
}


function onCreateOfferSuccess(desc) {
  local_sdp = desc.sdp;
  Connect();
  pc1.setLocalDescription(desc).then(
    function() {
      console.log("setLocalDescription succeed!");
    },
    function(){
      console.log("setLocalDescription error!");
    }
  );
}
function onCreateSessionDescriptionError(error) {
}

function gotRemoteStream(e) {

  if (remoteVideo.srcObject !== e.streams[0]) {
    console.log("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", e.streams);

    remoteVideo.srcObject = e.streams[0];
  }
}


function OnStreamGenerated(stream) 
{
  localVideo.srcObject = stream;
  localStream = stream;
  //remoteVideo = stream;
  var servers = null;
  pc1 = new RTCPeerConnection(servers);
  pc1.onicecandidate = function(e) {
    if (e.candidate) {
      // Send the candidate to the remote peer
      console.log("onicecandidate", e.candidate);
      //local_sdp = local_sdp + 'a=' + e.candidate.candidate;
      ice = e.candidate;
    } else {
      // All ICE candidates have been sent
      console.log("onicecandidate finished");
      //Connect();
    }
  };
  pc1.oniceconnectionstatechange = function(e) {
    console.log("oniceconnectionstatechange", pc1.iceConnectionState);
  };
  pc1.ontrack = gotRemoteStream;

  localStream.getTracks().forEach(
    function(track) {
      pc1.addTrack(
        track,
        localStream
      );
    }
  );
  var offerOptions = {
    offerToReceiveAudio: 0,
    offerToReceiveVideo: 1
  };
  pc1.createOffer(
    offerOptions
  ).then(
    onCreateOfferSuccess,
    onCreateSessionDescriptionError
  );
}

function start() {
  navigator.mediaDevices.getUserMedia({
    audio: false,
    video: true
  })
  .then(OnStreamGenerated)
  .catch(function(e) {
  });
}


start();







                
