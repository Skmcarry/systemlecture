var express = require('express')
var app = express()
fs = require('fs');
mysql = require('mysql');
var connection = mysql.createConnection({
    host: 'localhost',
    user: 'sensor',
    password: 'kkw0506',
    database: 'data'
})
connection.connect();


app.get('/graph', function (req, res) {
    console.log('got app.get(graph)');
    var html = fs.readFile('./graph.html', function (err, html) {
    html = " "+ html
    console.log('read file');

    var qstr = 'select * from sensors ';
    connection.query(qstr, function(err, rows, cols) {
      if (err) throw err;

      var data = "";
      var comma = ""

      for (var i=0; i< rows.length; i++) {
         r = rows[i];
	 tmp = r.time;
 	 console.log(r.value);
	
         data += comma + "[new Date("+r.time.getYear()+","+r.time.getMonth()+","+r.time.getDate()+","+r.time.getHours()+","+r.time.getMinutes()+","+r.time.getSeconds()+"),"+ r.value +"]";
         comma = ",";
      }
      var header = "data.addColumn('date', 'Time');"
      header += "data.addColumn('number', 'Temperature');"
      html = html.replace("<%HEADER%>", header);
      html = html.replace("<%DATA%>", data);

      res.writeHeader(200, {"Content-Type": "text/html"});
      res.write(html);
      res.end();
    });
  });
})

app.listen(3000, function () {
  console.log('Example app listening on port 3000!')
})
