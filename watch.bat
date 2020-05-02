@ECHO OFF
set time=%1
shift
set params=%1
:param_loop
shift
if [%1]==[] goto after_param_loop
set params=%params% %1
goto param_loop
:after_param_loop

:loop_watch
  cls
  %params%
  powershell -command "Start-Sleep -Milliseconds %time%">nul
goto loop_watch