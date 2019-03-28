var express = require("express");
var app = express();
var bodyParser = require("body-parser");

// 创建 application/x-www-form-urlencoded 编码解析
var urlencodedParser = bodyParser.urlencoded({ extended: false });

app.use(express.static("public"));

app.get("/", function(req, res) {
  res.sendFile(__dirname + "/" + "index.htm");
});

app.post("/post_data", urlencodedParser, function(req, res) {
  // 输出 JSON 格式
  response = {
    first_name: req.body.first_name,
    last_name: req.body.last_name
  };
  console.log(response);
  res.end(JSON.stringify(response));
});

var server = app.listen(8081, function() {
  var host = server.address().address;
  var port = server.address().port;

  console.log("server started @%s:%s", host, port);
});
