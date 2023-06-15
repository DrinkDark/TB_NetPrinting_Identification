@echo off
title PaperCut commands
cd C:\Program Files\PaperCut MF\server\bin\win
goto menu

:menu
cls
echo All commands available :
echo 1. Get user property
echo 2. Set user property
echo 3. List user account
echo 4. Add new user
echo 5. Delete user
set /p command= Choose a command : 
if "%command%" == "1" goto getUserProperty
if "%command%" == "2" goto setUserProperty 
if "%command%" == "3" goto listUserAccount
if "%command%" == "4" goto addNewUser
if "%command%" == "5" goto deleteUser

:getUserProperty
cls
echo Get user property
echo List of properties : balance, primary-card-number, secondary-card-number
set /p name= Enter user name : 
set /p property= Enter property : 
server-command get-user-property %name% %property%
pause
goto menu

:setUserProperty
cls
echo Set user property
echo List of properties : balance, primary-card-number, secondary-card-number
set /p name= Enter user name : 
set /p property= Enter property : 
set /p value= Enter value : 
server-command set-user-property %name% %property% %value%
pause
goto menu

:listUserAccount
cls
echo List of accounts
server-command list-user-accounts
pause
goto menu

:addNewUser
cls
echo Add new user
set /p name= Enter user name : 
set /p pCardN= Enter primary card number : 
set /p sCardN= Enter secondary card number :  
set /p balance= Enter balance :  
server-command add-new-user %name%
server-command set-user-property %name% balance %balance%
server-command set-user-property %name% primary-card-number %pCardN%
server-command set-user-property %name% secondary-card-number %sCardN%
goto menu

:deleteUser
cls
echo Delete user
set /p name= Enter user name : 
server-command delete-existing-user %name%
goto menu