set PLATFORM_SPR aeag100_plt/platform.spr
set VITIS_WORKSPACE [lindex $argv 0]
set XSA [lindex $argv 1]
puts "platform read $PLATFORM_SPR"
platform read $PLATFORM_SPR
puts "platform active"
platform active {aeag100_plt}
puts "platform config -updatehw  $XSA"
platform config -updatehw  $XSA
puts "platform generate"
platform generate
puts "setws $VITIS_WORKSPACE"
setws $VITIS_WORKSPACE
puts "app clean -name aeag100_app"
app clean -name aeag100_app
puts "app build -name aeag100_app"
app build -name aeag100_app
puts "build finish"