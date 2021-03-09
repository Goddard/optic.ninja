size_array=(8 16 22 24 32 36 42 48 64 72 96 128 192 256 512)

for i in "${size_array[@]}"
do
  convert $1 -resize "$i"x"$i" on-"$i".png
  echo "$i converted"
  sudo cp on-"$i".png /usr/share/icons/hicolor/"$i"x"$i"/apps/opticninja.png
  echo "copied"
done

sudo update-icon-caches /usr/share/icons/*
