Blinkenwall = function() {
	var websocket;
	
	this.doConnect = function() {
		if (window.MozWebSocket)
		{
			this.logToConsole('<span style="color: red;"><strong>Info:</strong> This browser supports WebSocket using the MozWebSocket constructor</span>');
			window.WebSocket = window.MozWebSocket;
		}
		else if (!window.WebSocket)
		{
			this.logToConsole('<span style="color: red;"><strong>Error:</strong> This browser does not have support for WebSocket</span>');
			return false;
		}
		
		// prefer text messages
		var uri = $("#wsUri").val();
		if (uri.indexOf("?") == -1) {
			//uri += "?encoding=text";
		} else {
			//uri += "&encoding=text";
		}
		var that = this;
		this.websocket = new WebSocket(uri);
		this.websocket.onopen = function(evt) { that.onOpen(evt) };
		this.websocket.onclose = function(evt) { that.onClose(evt) };
		this.websocket.onmessage = function(evt) { that.onMessage(evt) };
		this.websocket.onerror = function(evt) { that.onError(evt) };
		
		return false;
	};
	this.doDisconnect = function() {
		this.websocket.close();
		return false;
	};
	this.doSend = function() {
		this.logToConsole("SENT: " + sendMessage.value);
		this.websocket.send(sendMessage.value);
	
		return false;
	};

	this.domySend = function() {
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
		this.logToConsole("SENT MY: " + tmp.join(" "));
		this.websocket.send(tmp.join(" "));
	
		return false;
	};

	this.domySend2 = function() {
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
		this.logToConsole("SENT MY: " + tmp.join(" "));
		this.websocket.send(tmp.join(" "));
	
		return false;
	};
	
	this.logToConsole = function(message) {
		var tr = $('<tr></tr>');
		$("<td></td>").html(message).appendTo(tr);
		tr.appendTo("#consoleLog");
		
		var consoleLog = $("#consoleLog");
		
		while (consoleLog.children('tr').length > 50)
		{
			consoleLog.removeChild(consoleLog.firstChild('tr'));
		}
		
		consoleLog.scrollTop(consoleLog.scrollHeight);
	};

	this.onOpen = function(evt) {
		this.logToConsole("CONNECTED");
		this.setGuiConnected(true);
	};
	this.onClose = function(evt) {
		this.logToConsole("DISCONNECTED");
		this.setGuiConnected(false);
	};
	
	this.onMessage = function(evt) {
		this.logToConsole('<span style="color: blue;">RESPONSE: ' + evt.data+'</span>');
	}
	
	this.onError = function(evt) {
		this.logToConsole('<span style="color: red;">ERROR:</span> ' + evt.data);
	};
	  
	this.setGuiConnected = function(isConnected) {
		$('#wsUri').prop("disabled", isConnected);
		$('#connect').prop("disabled", isConnected);
		$('#disconnect').prop("disabled", !isConnected);
		$('#sendMessage').prop("disabled", !isConnected);
		$('#send').prop("disabled", !isConnected);
		$('#mysend').prop("disabled", !isConnected);
		$('#mysend2').prop("disabled", !isConnected);
	};
		
	this.clearLog = function() {
		$("#consoleLog").empty();
	};
};

$(function() {
	if (window.WebSocket)
		$("#webSocketSupp").prop("display", "block");
	else
		$("#noWebSocketSupp").prop("display", "block");
	
	var blinkenwall = new Blinkenwall();
	
	$("#wsUri").val("ws://10.20.30.193:15632/raw");
	
	$("#connect").click(function() { return blinkenwall.doConnect(); });
	$("#disconnect").click(function() { return blinkenwall.doDisconnect(); });
	$("#send").click(function() { return blinkenwall.doSend(); });
	$("#clearLogBut").click(function() { return blinkenwall.clearLog(); });
	$("#mysend").click(function() { return blinkenwall.domySend(); });
	$("#mysend2").click(function() { return blinkenwall.domySend2(); });
	
	blinkenwall.setGuiConnected(false);
});
