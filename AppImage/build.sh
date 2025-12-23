# bc im lazy

# colour defs bc why not
GREEN='\e[92m'
CYAN='\e[96m'
RESET='\e[0m'

# user input
echo -e "${GREEN}App icons:${RESET}"
ls "./AppDir/icons/" -1 | cat

printf "Icon name: ${GREEN}./AppDir/icons/${CYAN}"
read NAME
printf "${RESET}\n"

# make the .DirIcon symlink
ln -s "./icons/$NAME" "./.DirIcon"
mv -f "./.DirIcon" "./AppDir/.DirIcon"

# build it
ARCH=x86_64 ./appimagetool-x86_64.AppImage ./AppDir RSDKv4.AppImage
chmod +x RSDKv4.AppImage

echo "Press any key to continue..."
read -n1 -s -r