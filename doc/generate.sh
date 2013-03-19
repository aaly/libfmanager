mkdir html
for file in $(ls man); do roffit < man/$file > html/${file/.3/.html}; done
mkdir pdf
for file in $(ls man); do man -t man/$file | ps2pdf - > pdf/${file/.3/.pdf}; done
