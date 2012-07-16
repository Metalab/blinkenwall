var websocket;

$(function() {
	if (window.WebSocket)
	{
		document.getElementById("webSocketSupp").style.display = "block";
	}
	else
	{
		document.getElementById("noWebSocketSupp").style.display = "block";
	}
	
	$("#wsUri").val("ws://10.20.30.193:15632/raw");
	
	$("#connect").click(doConnect);
	$("#disconnect").click(doDisconnect);
	$("#send").click(doSend);
	$("#clearLogBut").click(clearLog);
	
	setGuiConnected(false);
});

function doConnect()
{
	console.log("doConnect");
	if (window.MozWebSocket)
	{
		logToConsole('<span style="color: red;"><strong>Info:</strong> This browser supports WebSocket using the MozWebSocket constructor</span>');
		window.WebSocket = window.MozWebSocket;
	}
	else if (!window.WebSocket)
	{
		logToConsole('<span style="color: red;"><strong>Error:</strong> This browser does not have support for WebSocket</span>');
		return false;
	}
	
	// prefer text messages
	var uri = $("#wsUri").val();
	if (uri.indexOf("?") == -1) {
		//uri += "?encoding=text";
	} else {
		//uri += "&encoding=text";
	}
	websocket = new WebSocket(uri);
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
	
	return false;
}

function doDisconnect()
{
	websocket.close()

	return false;
}

function doSend()
{
	logToConsole("SENT: " + sendMessage.value);
	websocket.send(sendMessage.value);

	return false;
}

  
function domySend()
{
	var tmp=new Array();
	for (i=0; i<18; i++){
		tmp.push(0xff.toString(16));
		tmp.push(0x0.toString(16));
		tmp.push(0x0.toString(16));
	}
	
	for (i=0; i<9; i++){
		tmp.push(0x00.toString(16));
		tmp.push(0xff.toString(16));
		tmp.push(0x00.toString(16));
	}
	
	for (i=0; i<18; i++){
		tmp.push(0x00.toString(16));
		tmp.push(0x00.toString(16));
		tmp.push(0xff.toString(16));
	}
	logToConsole("SENT MY: " + tmp.join(" "));
	websocket.send(tmp.join(" "));

	return false;
}
  
  
  
function domySend2()
{
	var tmp=new Array();
	for (i=0; i<9; i++){
		tmp.push(0x00.toString(16));
		tmp.push(0xff.toString(16));
		tmp.push(0x00.toString(16));
	}
	
	for (i=0; i<18; i++){
		tmp.push(0x00.toString(16));
		tmp.push(0x00.toString(16));
		tmp.push(0xff.toString(16));
	}
	
	for (i=0; i<18; i++){
		tmp.push(0xff.toString(16));
		tmp.push(0x0.toString(16));
		tmp.push(0x0.toString(16));
	}
	logToConsole("SENT MY: " + tmp.join(" "));
	websocket.send(tmp.join(" "));

	return false;
}
  
  
  
function logToConsole(message)
{
	var tr = $('<tr></tr>');
	$("<td></td>").html(message).appendTo(tr);
	tr.appendTo("#consoleLog");
	
	var consoleLog = $("#consoleLog");
	
	while (consoleLog.children('tr').length > 50)
	{
		consoleLog.removeChild(consoleLog.firstChild('tr'));
	}
	
	consoleLog.scrollTop(consoleLog.scrollHeight);
}
  
function onOpen(evt)
{
	logToConsole("CONNECTED");
	setGuiConnected(true);
}

function onClose(evt)
{
	logToConsole("DISCONNECTED");
	setGuiConnected(false);
}

function onMessage(evt)
{
	logToConsole('<span style="color: blue;">RESPONSE: ' + evt.data+'</span>');
}

function onError(evt)
{
	logToConsole('<span style="color: red;">ERROR:</span> ' + evt.data);
}
  
function setGuiConnected(isConnected)
{
	$('#wsUri').prop("disabled", isConnected);
	$('#connect').prop("disabled", isConnected);
	$('#disconnect').prop("disabled", !isConnected);
	$('#sendMessage').prop("disabled", !isConnected);
	$('#send').prop("disabled", !isConnected);
}
	
function clearLog()
{
	$("#consoleLog").empty();
}
