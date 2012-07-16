var wsUri;
var consoleLog;
var connectBut;
var disconnectBut;
var sendMessage;
var sendBut;
var clearLogBut;

function echoHandlePageLoad()
{
	if (window.WebSocket)
	{
		document.getElementById("webSocketSupp").style.display = "block";
	}
	else
	{
		document.getElementById("noWebSocketSupp").style.display = "block";
	}
	
	wsUri = document.getElementById("wsUri");
	wsUri.value = "ws://10.20.30.193:15632/raw";
	
	connectBut = document.getElementById("connect");
	connectBut.onclick = doConnect;
	
	disconnectBut = document.getElementById("disconnect");
	disconnectBut.onclick = doDisconnect;
	
	sendMessage = document.getElementById("sendMessage");
	
	sendBut = document.getElementById("send");
	sendBut.onclick = doSend;
	
	consoleLog = document.getElementById("consoleLog");
	
	clearLogBut = document.getElementById("clearLogBut");
	clearLogBut.onclick = clearLog;
	
	setGuiConnected(false);
	
	document.getElementById("disconnect").onclick = doDisconnect;
	document.getElementById("send").onclick = doSend;
	
}

function doConnect()
{
	if (window.MozWebSocket)
	{
		logToConsole('<span style="color: red;"><strong>Info:</strong> This browser supports WebSocket using the MozWebSocket constructor</span>');
		window.WebSocket = window.MozWebSocket;
	}
	else if (!window.WebSocket)
	{
		logToConsole('<span style="color: red;"><strong>Error:</strong> This browser does not have support for WebSocket</span>');
		return;
	}
	
	// prefer text messages
	var uri = wsUri.value;
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
}

function doDisconnect()
{
	websocket.close()
}

function doSend()
{
	logToConsole("SENT: " + sendMessage.value);
	websocket.send(sendMessage.value);
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
}
  
  
  
function logToConsole(message)
{
	var pre = document.createElement("p");
	pre.style.wordWrap = "break-word";
	pre.innerHTML = message;
	consoleLog.appendChild(pre);
	
	while (consoleLog.childNodes.length > 50)
	{
		consoleLog.removeChild(consoleLog.firstChild);
	}
	
	consoleLog.scrollTop = consoleLog.scrollHeight;
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
	wsUri.disabled = isConnected;
	connectBut.disabled = isConnected;
	disconnectBut.disabled = !isConnected;
	sendMessage.disabled = !isConnected;
	sendBut.disabled = !isConnected;
}
	
function clearLog()
{
	while (consoleLog.childNodes.length > 0)
	{
		consoleLog.removeChild(consoleLog.lastChild);
	}
}
  
window.addEventListener("load", echoHandlePageLoad, false);
