echo "====== Push begin... ======"
read -p "Notes: " note
git add .
git commit -m "${note}"
git push origin master
if [ ! -z $? ]; then
    git pull origin master
    git add .
    git commit -m "${note}"
    git push origin master
fi
echo "====== Push finished ======"