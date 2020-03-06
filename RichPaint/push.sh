echo "====== Push begin... ======"
read -p "Notes: " note
git add .
git commit -m "${note}"
git push origin master
echo "====== Push finished ======"