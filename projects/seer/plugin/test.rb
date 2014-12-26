
UI.messagebox("Starting test.rb")

def add_point(point)
  UI.messagebox("Point added " + String(point[0]) + ", " + String(point[1]) + ", " + String(point[2]))
  model = Sketchup.active_model
  pt = Geom::Point3d.new(point[0],point[1],point[2])
  model.entities.add_cpoint(pt)
  model.commit_operation

  # may work:  model.entities.add_cpoint(point)
  # UI.messagebox("Point added " + point[0] + "," + point[1] + "," + point[2])
end


=begin

add_point([0.83, 1.30, -0.53])

interval = 3
@timer_id = UI.start_timer(interval, true) do
  add_point([0.36, 1.16, -0.42])
  UI.stop_timer(@timer_id) 
end

=end


source = "/home/charguer/shared/ardunio/projects/seer/plugin/log.txt"
$myfile = File.new(source, "r")
$mybuffer = ""
$mypos = 0


def check_input
  $myfile.seek($mypos, IO::SEEK_SET)
  # UI.messagebox($mypos)
  until $myfile.eof?
    char = $myfile.read(1)
    $mypos += 1
    $mybuffer = $mybuffer + char
    if char == "\n"
      # UI.messagebox("adding point: " + $mybuffer)
      # UI.messagebox($mybuffer)
      words = $mybuffer.split(" ")
      if words[0] == "#"
        # UI.messagebox("adding point: " + $mybuffer)
        point = [ 10*Float(words[1]), 10*Float(words[2]), 10*Float(words[3])] 
        add_point(point)
      end
      $mybuffer = ""
    end
  end
end

interval = 0.1
@timer_id = UI.start_timer(interval, true) do
  # UI.messagebox("checking")
  check_input
end


###################################################
# when closing the program

=begin
  file.close()
  UI.stop_timer(@timer_id) 
=end



