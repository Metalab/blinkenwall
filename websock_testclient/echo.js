  var secureCb;
  var secureCbLabel;
  var wsUri;
  var consoleLog;
  var connectBut;
  var disconnectBut;
  var sendMessage;
  var sendBut;
  var clearLogBut;

  function echoHandlePageLoad()
  {



    
    
    
    wsUri = document.getElementById("wsUri");
    SetURI();
    
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

  function SetURI()
  {
   
	wsUri.value = "ws://10.20.30.193:15632/raw";

  }
  
  function doConnect()
  {


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
tmp=tmp+0xff.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
}

for (i=0; i<9; i++){
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0xff.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
}

for (i=0; i<18; i++){
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0xff.toString(16)+" ";
}
    logToConsole("SENT MY: " + tmp);
    websocket.send(tmp);
  }
  
  
  
      function domySend2()
  {
	  var tmp=new Array();


for (i=0; i<9; i++){
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0xff.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
}

for (i=0; i<18; i++){
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0xff.toString(16)+" ";
}
	  for (i=0; i<18; i++){
tmp=tmp+0xff.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
tmp=tmp+0x00.toString(16)+" ";
}
    logToConsole("SENT MY: " + tmp);
    websocket.send(tmp);
  }
  
  
  
  function logToConsole(message)
  {
    var pre = document.createElement("p");
    pre.style.wordWrap = "break-word";
    //pre.innerHTML = getSecureTag()+message;
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
   // secureCb.disabled = isConnected;
    var labelColor = "black";
    if (isConnected)
    {
      labelColor = "#999999";
    }
     
    
  }
	
	function clearLog()
	{
		while (consoleLog.childNodes.length > 0)
		{
			consoleLog.removeChild(consoleLog.lastChild);
		}
	}
	
/*	function getSecureTag()
	{
		if (secureCb.checked)
		{
			return '<img src="img/tls-lock.png" width="6px" height="9px"> ';
		}
		else
		{
			return '';
		}
	}*/
  
  window.addEventListener("load", echoHandlePageLoad, false);
