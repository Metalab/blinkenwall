var currentFrameNumber=0;
var frameBuffer = new Array();
var animation_running=0;
var default_timer=500;


colorlist= [
"000000",
"FFFFFF",
"00FF99",
"00FF00",
"99FF00",
"FFFF00",
"FF9900",
"FF0000",
"FF0099",
"FF00FF",
"9900FF",
"0000FF",
"0099FF",
"00FFFF",
];



function setPaintColor(elm)
{

 $('#colorpicker').children().css("border","1px solid white");
 
 elm_rgb=elm.css("backgroundColor");
 elm.css("border","1px solid red");
 hexval=RGBToHex(elm_rgb);
 $("#currentcolor_yay").val(hexval);
}
     
     



function setBrick(brick,color)
{
 brick.css('backgroundColor', '#' + color);
}



function paintAll(color)
{
 $("td").css('backgroundColor', '#' + color);
}






function saveCurrentFrame()
{
 currentFrameList = getAllBrickColors();
 if(!frameBuffer[currentFrameNumber])
  frameBuffer[currentFrameNumber]=new Array();

 frameBuffer[currentFrameNumber][0]=$("#frametimer").val();
 frameBuffer[currentFrameNumber][1]=currentFrameList;


// alert(frameBuffer[currentFrameNumber][0]); 
}


function loadFrame(frameNumber)
{
 if(frameBuffer[frameNumber]) 
 {
   $("#frametimer").val(frameBuffer[frameNumber][0]);
   drawFrame(frameBuffer[frameNumber][1]);
 }
 else
 {
  // maybe we should fill the frameBuffer here?
  paintAll("000000");
  $("#frametimer").val(default_timer); 
  saveCurrentFrame();
 }
updateFrameCounter();
}



function copyFrame()
{
 loadFrame(currentFrameNumber-1);
 saveCurrentFrame();
}


function drawFrame(frameList)
{
  tiles = $("#blinkentable").find("td");
  for (var i = 0; i < tiles.length; i++) {
   $(tiles[i]).css("backgroundColor","#"+frameList[i]);
  }
 

}


function nextFrame()
{
 saveCurrentFrame();
 currentFrameNumber+=1;
 loadFrame(currentFrameNumber); 
}


function previousFrame()
{
 
 saveCurrentFrame();
 currentFrameNumber-=1;
 loadFrame(currentFrameNumber);
}


function updateFrameCounter()
{
 totalFrames = frameBuffer.length;
 $("#framecounter").val((currentFrameNumber+1)+"/"+totalFrames);

 // enable/disable PREV  button
 if(currentFrameNumber==0) { 
  $("#prevframe").attr("disabled","disabled"); 
  $("#copyframe").attr("disabled","disabled"); 
}
 else {
  $("#prevframe").removeAttr("disabled");
  $("#copyframe").removeAttr("disabled");

} 

}






function deleteFrame()
{
 frameBuffer.splice(currentFrameNumber,1);
 
 if(currentFrameNumber>0)
  currentFrameNumber-=1;


 loadFrame(currentFrameNumber);  
 

}











function getAllBrickColors()
{
  var list = new Array();
  tiles = $("#blinkentable").find("td");
  for (var i = 0; i < tiles.length; i++) {
   list[i] = (RGBToHex($(tiles[i]).css("backgroundColor")));
  }
  return list;
}




function playAnimation()
{

 if(!animation_running)
 {
  // if we are at the last frame we have to save here to append to the frameBuffer 
  saveCurrentFrame();

  $("#playanimation").val("STOP animation playback");
  $(".hideableoptions").hide(500);
  animation_running=1;

  currentFrameNumber=0;
  animateFrame(currentFrameNumber);
 }
 else
 {
  $(".hideableoptions").show(200);
  $("#playanimation").val("Show animation in browser");
  animation_running=0;
 }
}
 

function animateFrame(frameNumber)
{
  if(animation_running)
  {

   currentFrameNumber=frameNumber;
   loadFrame(currentFrameNumber);

   // next frame or first frame depending on current frame
   showframe = (frameNumber<(frameBuffer.length-1)) ? (frameNumber+1) : 0;

  // useful comment is useful
  //                         NEXT frame    IN time from current frame!
   setTimeout("animateFrame("+showframe+")",frameBuffer[(frameNumber)][0]);
  }
}





//  MOVE frames around



function moveUp()
{
 saveCurrentFrame();

 for (var x=0;x<4;x++)
 {
  for (var y=0;y<9;y++)
  {
   frameBuffer[currentFrameNumber][1][(x*9+y)]=frameBuffer[currentFrameNumber][1][(x+1)*9+y];
  }
 }
 for (var z=36;z<45;z++)
  frameBuffer[currentFrameNumber][1][z]="000000";

 loadFrame(currentFrameNumber);
 saveCurrentFrame();

}



function moveDown()
{
 saveCurrentFrame();
 
 for (var x=3;x>=0;x--)
 {
  for (var y=0;y<9;y++)
  {
   frameBuffer[currentFrameNumber][1][(x+1)*9+y]=frameBuffer[currentFrameNumber][1][(x*9+y)];
  }
 }

 for (var z=0;z<9;z++)
  frameBuffer[currentFrameNumber][1][z]="000000";
 
 loadFrame(currentFrameNumber);
 saveCurrentFrame();

}





function moveLeft()
{
 saveCurrentFrame();

 for (var x=0;x<5;x++)
 {
  for (var y=0;y<8;y++)
  {
   frameBuffer[currentFrameNumber][1][x*9+y]=frameBuffer[currentFrameNumber][1][(x*9+y+1)]
  }
 }

 for (var x=0;x<5;x++)
  frameBuffer[currentFrameNumber][1][(x*9)+8]="000000";

 loadFrame(currentFrameNumber);
 saveCurrentFrame();

}


function moveRight()
{
 saveCurrentFrame();

 for (var x=0;x<5;x++)
 {
  for (var y=7;y>=0;y--)
  {
   frameBuffer[currentFrameNumber][1][x*9+y+1]=frameBuffer[currentFrameNumber][1][(x*9+y)]
  }
 }

 for (var x=0;x<5;x++)
  frameBuffer[currentFrameNumber][1][(x*9)]="000000";

 loadFrame(currentFrameNumber);
 saveCurrentFrame();

}






// load animations from server and update dropdown!
function  getAnimsFromServer()
{
     $.get("cgi-bin/get_animations.cgi", {}, function(data)
     { 
       entries = data.split("\n");
       for (var x=0; x<(entries.length-1);x++)
       {
        name_author_frames = entries[x].split(".");

        // set the very first entry once..
        if(!x) 
        {
         $("#authordisplay").html(name_author_frames[1]);
         $("#framesno").html(name_author_frames[2]);
        }

        $("#animationsonserverdropdown").append("<option value='"+entries[x]+"'>"+name_author_frames[0]+" ("+name_author_frames[2]+")"+"</option>")
       }
      
$("#animationsonserverdropdown").change(function(e){ 
         n_a = $(this).val().split(".");
         $("#authordisplay").html(n_a[1]);
         $("#framesno").html(n_a[2]);
        });



   $("#loadoptions").show();
   } );


}



function loadAnimation()
{

  animname = $("#animationsonserverdropdown").val();
  $.get("animations/"+animname, {}, function(animdata)
  {
   frames=animdata.split("\n");
   for (var i=0; i<(frames.length-1);i++)
   {


     content=frames[i].split(" ");
     frameArray= new Array();
     

     if(!frameBuffer[i])
       frameBuffer[i]=new Array();
     frameBuffer[i][0]=parseInt(content.shift());
     frameBuffer[i][1]=content;
    
   }

    currentFrameNumber=0;
    loadFrame(currentFrameNumber);


  });

}





///  save stuff

function saveAnimation(){
 saveCurrentFrame();
 $("#animation").val( buildAnimationString() );
 $("#framenumbercounter").val( frameBuffer.length );

 $("#saveform").submit();

}

function buildAnimationString()
{
 anim="";
 for (var x=0; x<frameBuffer.length;x++)
 {
  timeprefix="";
  for(var l=(frameBuffer[x][0]+"").length; l<7 ;l++)
    timeprefix+="0";
 
 anim+=timeprefix+frameBuffer[x][0];
 
 for (var y=0; y<frameBuffer[x][1].length;y++)
    anim+=" "+frameBuffer[x][1][y];
 anim+="\n";
 }
 return anim;
}


// CLEAR animation
function clearAnimation()
{

 currentFrameNumber=0;
 frameBuffer.splice(currentFrameNumber,(frameBuffer.length-1));
 loadFrame(currentFrameNumber);
}






function checkTimerValue()
{
  // only numbers in timer plz! 

 val = $("#frametimer").val();

  if( ! (parseInt(val,10)==val && (val.length>=1) && val.length<=5))
    $("#frametimer").val("500");

}





function playAnimationOnWall()
{
 // setz die checkbox damit das ding nicht permanent gespeichert bleibt..
 $("#tempanim").attr('checked', true);
 saveAnimation();
 $("#tempanim").attr('checked', false);

}


function RGBToHex(rgb) {
   str = rgb.replace(/rgb\(|\)/g, "").split(",");
   str[0] = parseInt(str[0], 10).toString(16).toLowerCase();
   str[1] = parseInt(str[1], 10).toString(16).toLowerCase();
   str[2] = parseInt(str[2], 10).toString(16).toLowerCase();
   str[0] = (str[0].length == 1) ? '0' + str[0] : str[0];
   str[1] = (str[1].length == 1) ? '0' + str[1] : str[1];
   str[2] = (str[2].length == 1) ? '0' + str[2] : str[2];
   return (str.join(""));
}



function askBeforeDelete()
{
 if(frameBuffer.length)
  return confirm('You will loose your current Animation.\n Are you sure?');
 else
   return true;
}


$(document).ready(function() {
   // do stuff when DOM is ready
   
   //   // global uglyness :)
        document.onmousedown = function(){mouseIsDown = true;};
        document.onmouseup =  function(){mouseIsDown = false;};
        var mouseIsDown = false;
   
  
        // attach the colorpicker
//        $('#cp_base').ColorPicker({flat: true});

        for (var i=0;i<colorlist.length;i++)
                  $('#colorpicker').append("<li id='"+colorlist[i]+"' style='background-color:#"+colorlist[i]+"'></li>");

     

       // initial green 
        setPaintColor($("#00FF00"));
        $('#framecounter').val("1/1"); 


        $('#colorpicker').children().click(function(e){setPaintColor($(this))});
  
                         
                         

        // bind painting functions on buttons
        $("#paintall").click(function(){paintAll($("#currentcolor_yay").val());});
        $("#clearall").click(function(){paintAll("000000");});
 
        // animation functions binder
        $("#frametimer").change(function(){checkTimerValue();});
        $("#nextframe").click(function(){nextFrame();});
        $("#prevframe").click(function(){previousFrame();});
        $("#deleteframe").click(function(){deleteFrame();});
        $("#copyframe").click(function(){copyFrame();});        
        $("#playanimation").click(function(){playAnimation();}); 


        $("#playtempanimation").click(function(){playAnimationOnWall();});
 

       //  move frames around
       $("#moveup").click(function(){moveUp();});
       $("#moveleft").click(function(){moveLeft();});
       $("#moveright").click(function(){moveRight();}); 
       $("#movedown").click(function(){moveDown();}); 



       //load animations
       $("#loadsubmit").click(function(){if(askBeforeDelete()){loadAnimation();}});

       // save it
       $("#saveanimation").click(function(){saveAnimation();});
       $("#saveform").ajaxForm(function() { 
                alert("Upload finished successfully"); 
            }); 
       $("#clearanimation").click(function(){if(askBeforeDelete()){clearAnimation();}});





 
        // get animations from server and fill dropdown
        getAnimsFromServer();



        // next 2 binduings for "masspainting"  just click and move your mouse around..
        $("td").mouseover(function() {
         if(mouseIsDown)    setBrick($(this),$("#currentcolor_yay").val()); 
        });
        $("td").mouseout(function() {
         if(mouseIsDown) setBrick($(this),$("#currentcolor_yay").val());
        });





        $("td").mousedown(function(e) {

  	  if(e.ctrlKey)
          {
           // use color of theis brick
           $("#currentcolor_yay").val(RGBToHex($(this).css("backgroundColor")));
           return;
          }        
       

          // set to black onClick 
          color="000000";
      
          // but wait.. if we are black already we should take some nice color from selection!
          if(RGBToHex($(this).css("backgroundColor"))=="000000") { 
           color=$("#currentcolor_yay").val(); 
          }


          setBrick($(this),color);
        });





});

