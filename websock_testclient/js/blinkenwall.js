Blinkenwall = function() {
	var websocket;
	var myUUID
	
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
		var that = this;
		this.websocket = new WebSocket($("#wsUri").val());
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
	
	
	this.renewRandomUUID = function() {

		var s = [], itoh = '0123456789ABCDEF';
		
		// Make array of random hex digits. The UUID only has 32 digits in it, but we
		// allocate an extra items to make room for the '-'s we'll be inserting.
		for (var i = 0; i <36; i++) s[i] = Math.floor(Math.random()*0x10);
		
		// Conform to RFC-4122, section 4.4
		s[14] = 4;  // Set 4 high bits of time_high field to version
		s[19] = (s[19] & 0x3) | 0x8;  // Specify 2 high bits of clock sequence
		
		// Convert to hex chars
		for (var i = 0; i <36; i++) s[i] = itoh[s[i]];
		
		// Insert '-'s
		s[8] = s[13] = s[18] = s[23] = '-';
		
		//return s.join('');
		myUUID=s.join('');
		this.logToConsole("Your new UUID is: " + myUUID);
		return false;
	}


	this.gamepadSend = function(keycode) {
		//var tmp=new Array();
		
		//tmp.push(0xff.toString(16));
		//tmp.push(0x0.toString(16));
		//tmp.push(0x0.toString(16));
		
		//this.logToConsole("Key Pressed: " + keycode);
		this.websocket.send("key " + myUUID + " " + keycode);
		//alert("key " + myUUID + " " + keycode);
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
		this.ParseServerCommand(evt.data);
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
		
		this.connected = isConnected;
	};
		
	this.clearLog = function() {
		$("#consoleLog").empty();
	};
	
	this.ParseServerCommand = function(message) {
		//alert(message);
		
		var tokens=message.split(" ");
		switch(tokens[0])
		{
		case "tetris":
			if (tokens[1]=="stone") {
				$("#ServerImage").prop("src", "http://10.20.30.150/gamefiles/tetris/" + tokens[2] + ".png");
				$("#ServerText").text("Nächster Stein");
			}
			break;
		default:
		}
		return false;
	};
};

$(function() {	


	if (window.WebSocket)
		$("#webSocketSupp").prop("display", "block");
	else
		$("#noWebSocketSupp").prop("display", "block");
	
	var blinkenwall = new Blinkenwall();
	
	//$("#wsUri").val("ws://10.20.30.176:15633/blinkenwallcontrol");
	$("#wsUri").val("ws://10.20.30.150:15633/blinkenwallcontrol");
	$("#connect").click(function() { return blinkenwall.doConnect(); });
	$("#disconnect").click(function() { return blinkenwall.doDisconnect(); });
	$("#send").click(function() { return blinkenwall.doSend(); });
	$("#clearLogBut").click(function() { return blinkenwall.clearLog(); });
	$("#mysend").click(function() { return blinkenwall.domySend(); });
	$("#mysend2").click(function() { return blinkenwall.domySend2(); });
	

	$("#btn-gamepad-up").click(function() { return blinkenwall.gamepadSend("77"); });
	$("#btn-gamepad-down").click(function() { return blinkenwall.gamepadSend("73"); });
	$("#btn-gamepad-left").click(function() { return blinkenwall.gamepadSend("61"); });
	$("#btn-gamepad-right").click(function() { return blinkenwall.gamepadSend("64"); });
	$("#btn-gamepad-1").click(function() { return blinkenwall.gamepadSend("71"); });
	$("#btn-gamepad-2").click(function() { return blinkenwall.gamepadSend("65"); });
	
	
	blinkenwall.renewRandomUUID();
	
	blinkenwall.setGuiConnected(false);
	
	
	//this is cool for debug
	blinkenwall.doConnect();
	
	$('body').on('keydown', function( e ){
		if(!blinkenwall.connected)
			return true;
		var key = ( window.event ) ? event.keyCode : e.keyCode;
		switch( key )
		{
			case 87: //w up
				return blinkenwall.gamepadSend("77")
				break;
			case 83: //s down
				blinkenwall.gamepadSend("73")
				break;
			case 65: //a left
				blinkenwall.gamepadSend("61")
				break;
			case 68: //d right
				blinkenwall.gamepadSend("64")
				break;
			case 81: //q 1
				blinkenwall.gamepadSend("71")
				break;
			case 69: //e 2
				blinkenwall.gamepadSend("65")
				break;
		}
		return false;
	});

	$('body').on('keyup', function( e ){
		if(!blinkenwall.connected)
			return true;
		var key = ( window.event ) ? event.keyCode : e.keyCode;
		switch( key )
		{
			case 87: //w up
				return blinkenwall.gamepadSend("7")
			break;
			case 83: //s down
				blinkenwall.gamepadSend("8")
				break;
			case 65: //a left
				blinkenwall.gamepadSend("9")
				break;
			case 68: //d right
				blinkenwall.gamepadSend("A")
				break;
			case 81: //q 1
				blinkenwall.gamepadSend("B")
				break;
			case 69: //e 2
				blinkenwall.gamepadSend("C")
				break;
		}
		return false;
	});
});
