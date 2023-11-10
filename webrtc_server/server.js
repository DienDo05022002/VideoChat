var wSocket = new WebSocket("ws://127.0.0.1:4567")

var peerid;
var localName;
var Users;
var type;
//var request = null;
//var hangingGet = null;
//var localName;
//var server;
//var my_id = -1;
//var other_peers = {};
//var message_counter = 0;

function connect() {
    localName = document.getElementById("localName").value;

    // Kiểm tra xem tên người dùng đã được nhập
    if (localName) {
        var signInData = {
            type: "signin",
            username: localName
        };
        wSocket.send(JSON.stringify(signInData));
    } else {
        alert("Please enter your name :)")
    }
}
wSocket.onmessage = function(event) {
  // Xử lý dữ liệu nhận được từ máy chủ WebSocket.
  var response = JSON.parse(event.data);
  console.log("Nhận dữ liệu từ máy chủ: " + JSON.stringify(response));
  // Xử lý dữ liệu theo cách mà bạn cần.
    // Lấy các thuộc tính cụ thể mà bạn quan tâm
  peerid = response.peerid;
  Users = response.username;
  type= response.type
  trace(peerid,type, Users)
};
function trace(peerid, type, Users) {
  var elem = document.getElementById("debug");
  elem.innerHTML += "<p>" + "ID:" + peerid +"  ,Type:" + type + "  ,Username:" + Users + "</p><br>";

}

