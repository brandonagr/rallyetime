# Introduction #

The goal of this system was to build a laptop rally system that completely replaced the roll of a navigator so I could run a rally by myself and win both the driver and nav championship.

### RallyeTime DG ###
RallyeTime DG is the version of this project hosted here. Older versions that rely on a GPS reciever for distance are on the Downloads page. The goal is to make a system that completely replaces the role of a navigator during a rallye. This allows one person to run the rallye drivagating (filling the roles of both a driver and a navigator). It allow this by interfacing with a small LCD screen that will display directions to the driver. There will also be button interface mounted within easy reach of the driver to allow them to tell the system when they have completed a direction. An [NI USB 6501](http://sine.ni.com/nips/cds/view/p/lang/en/nid/201630) DAQ box is used to interface all of the external electronics with the laptop. Designed to be used on rallyes run by the [Texas A&M Sports Car Club](http://tamscc.org)

You can [view pictures of the hardware build](http://s283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/) for this project.

[A general overview of some of the initial design goals can be found](http://code.google.com/p/rallyetime/wiki/Design)

## !Update: Rallye Champion! ##
The fall season has concluded and luckily I can report that this project was a success! However, due to an unexpected rule interpretation I was prevented from winning both the Driver and Nav championship so I had to fall back on only winning the Driver championship. My system never failed me on a single rally though, and the only thing that could of been better was knowing the calibration of the rally master's odometer ahead of time. In the future I hope to participate in some SCCA rallies, so the next iteration of this project will hopefully be more useful to more people.

Here is some media from the fall 2008 season:
  * [Video showing the first leg of the final rallye of the semester](http://www.youtube.com/watch?v=NPkjtrgg9Co)
  * [Rally 1 map](http://maps.google.com/maps?f=q&hl=en&geocode=&q=http:%2F%2Frallyetime.googlecode.com%2Fsvn%2FRallyeTimeDG%2Fbin%2Fdata%2Farchive%2Ff08_r1%2Ff08_r1.kmz&ie=UTF8&z=12)
  * [Rally 2 map](http://maps.google.com/maps?f=q&hl=en&geocode=&q=http:%2F%2Frallyetime.googlecode.com%2Fsvn%2FRallyeTimeDG%2Fbin%2Fdata%2Farchive%2Ff08_r2%2Ff08_r2.kmz&ie=UTF8&z=12)
  * [Rally 3 map](http://maps.google.com/maps?f=q&hl=en&geocode=&q=http:%2F%2Frallyetime.googlecode.com%2Fsvn%2FRallyeTimeDG%2Fbin%2Fdata%2Farchive%2Ff08_r3_runbysam%2Ff08_r3.kmz&ie=UTF8&z=12)
  * [Day Rally map](http://maps.google.com/maps?f=q&hl=en&geocode=&q=http:%2F%2Frallyetime.googlecode.com%2Fsvn%2FRallyeTimeDG%2Fbin%2Fdata%2Farchive%2Ff08_rday%2Ff08_rday.kmz&ie=UTF8&z=12)
  * [Rally 4 map](http://maps.google.com/maps?f=q&hl=en&geocode=&q=http:%2F%2Frallyetime.googlecode.com%2Fsvn%2FRallyeTimeDG%2Fbin%2Fdata%2Farchive%2Ff08_r4%2Ff08_r4.kmz&ie=UTF8&z=12)


You can also view a map of all the Fall

### Preparation for first rallye of the season ###
Here's some pics of the final product, which worked perfectly during the first rallye, allowing me to beat 34 out of 35 rallye competitors and allowing RallyeTime to take the top two places. Another team using the 4.1 version of RallyeTime was able to get first.


![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product006.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product006.jpg)

Getting ready for the rallye

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product012.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product012.jpg)

Installing and routing the auxiliary audio input connection to stereo

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/wheelsensor011.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/wheelsensor011.jpg)

Wheelspeed sensor off the front wheel to get an accurate measure of distance travelled

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product022.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product022.jpg)

GPS receiver to record the rallye for playback/stats/maps later

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product019.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product019.jpg)
![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product040.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product040.jpg)

Mounted in the back: Power switch, fuse, extra 12v outlet, NI DAQ box that interfaces with the wheelsensor and buttons, dc to dc converter that runs the laptop straight from the car battery, the laptop. Laptop connects to the GPS, scanner, keypad, DAQ box, car stereo, dc power

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product042.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product042.jpg)
![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product038.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product038.jpg)

I took out the passenger seat and mounted a scanner(an old one that I happened to have sitting around). That way I don't have to type in all the directions by hand, just scan them and use an OCR program such as AABBYY FineReader to convert it to a text file. The numberpad is used to enter leave times into the system. Carry a 1 million candle power spotlight just in case, the clipboard with the directions(backup in case the system fails) is velcroed down

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product029.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product029.jpg)

Button interface, there are buttons for next direction, reaching a checkpoint, and undo if you hit a button accidently. There's a panic flip switch for when you get lost. The laptop also [reads out the directions](http://wcarchive.cdrom.com/pub/bws/bws_44/Crystal16mp3.mp3) over the car stereo as it goes

![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product034.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product034.jpg)
![http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product033.jpg](http://i283.photobucket.com/albums/kk312/brandonagr/RallyeTime%20DG/final_product033.jpg)

The LCD display, displays 4 directions at a time, the current time off(tells me to go faster or slower), avg speed/cast, direction number, and current speed


#### Credits ####

I have been continually working on and improving this program over the years and lots of people have helped. Thanks to Ben, Kat, Sam, Darcie, Sam's Garage, Chad, and Russell for ideas, inspiration, and help testing.