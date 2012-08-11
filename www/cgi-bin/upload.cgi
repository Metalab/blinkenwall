#!/usr/local/bin/haserl
content-type: text/html





yay!
<?
//d=`date +%Y.%m.%d.%H.%M.%S`


$POST_animation





name=`echo -n "$POST_animname" | sed 's/[^a-zA-Z0-9]//g'`;
author=`echo -n $POST_animauthor | sed 's/[^a-zA-Z0-9]//g' `;
framenumbers=`echo -n $POST_framenumbers | sed 's/[^0-9]//g' `;

filename=../animations/$name.$author.$framenumbers;


if [ $POST_tempanim ]; then
 filename=/var/www/animations/playthisnow 
fi; 


touch $dirname
echo -n "$POST_animation" > $filename;

#IFS="
#"
#
#counter=0;
#for line in  $POST_animation;do
# counter=$(($counter+1));
# len=`length $counter`; 
#
# numprepend="";
# while [ $len -lt 5 ]						
# do	
#   len=$(($len+1)) 
#   numprepend=$numprepend"0"							
# done
# echo "L $numprepend$counter : $line<br>";
#
#
#done

exit
?>
