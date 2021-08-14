del ..\..\GUI\English.zxf
del ..\..\GUI\Chinese.zxf
rd /s /q ..\..\GUI\Release\
copy English.ini ..\..\GUI\English.zxf
copy Chinese.ini ..\..\GUI\Chinese.zxf
del "C:\SAtemp\language" /f /s /q