const WebSocket = require('ws');
 
const ws = new WebSocket('ws://172.16.16.194:8188', 'janus-protocol');
 
ws.on('open', function open() {
    console.log("ssssssssss");

    var json = {};
    json.janus = "create";
    json.transaction = "12345678";
    var jsonStr = JSON.stringify(json);
    ws.send(jsonStr);
});

var a = 0;
ws.on('message', function incoming(data) {
  console.log(data);


  if (a === 0) {
    var json_obj = JSON.parse(data);
    var id = json_obj.data.id;
    var json = {};
    json.janus = "attach";
    json.plugin  = "janus.plugin.echotest";
    json.transaction = "22345678";
    json.opaque_id = id;
    var jsonStr = JSON.stringify(json);
    ws.send(jsonStr);
    a = 1;
   }

});