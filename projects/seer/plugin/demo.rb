###################################################
# demo: read the file "log.txt" and report the
# points that are contained in the file,
# even supporting points added dynamically

def add_point(point)
  print ("x: " + String(point[0]) + "\t")
  print ("y: " + String(point[1]) + "\t")
  print ("z: " + String(point[2]) + "\t")
  print "\n"
end

source = "log.txt"
myfile = File.new(source, "r")
buffer = ""
$mypos = Integer(0)

def check_input(file, buffer)
  file.seek($mypos, IO::SEEK_SET)
  until file.eof?  
    char = file.read(1)
    $mypos += 1
    buffer = buffer + char
    if char == "\n"
      words = buffer.split(" ")
      point = [ words[0], words[1], words[2]] 
      add_point(point)
      buffer = ""
    end
  end
end


loop do
  check_input(myfile, buffer)
  sleep 0.5
end



# C:\Users\<your_windows_user_name>\AppData\Roaming\SketchUp\SketchUp [n]\SketchUp\Plugins

# minicom or cutecom on mac or http://automatica.com.au/2011/09/picocom-1-6-for-mac-osx-10-6/
# /usr/bin/picocom /dev/ttyACM0 > /home/charguer/temp/log.txt

# file.seek(0, IO::SEEK_END) # rewinds file to the end
#    http://wiki.cfcl.com/SketchUp/Cookbook/AT_Asynch_Input
