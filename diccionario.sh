sed -e 's/PeÑalolen/Peñalolen/g' PhoneBusStopsSequences_dtpm.csv > tmp;
sed -e 's/ÑuÑoa/Ñuñoa/g' tmp > tmp1;
sed -e 's/EspaÑa/España/g' tmp1 > tmp;
sed -e 's/Maipu/Maipú/g' tmp > tmp1;
sed -e 's/Satelite/Satélite/g' tmp1 > tmp;
sed -e 's/Joaquin/Joaquín/g' tmp > tmp1;
sed -e 's/Martin/Martín/g' tmp1 > tmp;
sed -e 's/\(M\) Pajarito/\(M\) Pajaritos/g' tmp > tmp1;
sed -e 's/Heroes/Héroes/g' tmp1 > tmp;
sed -e 's/U de Chile/U\. De Chile/g' tmp > tmp1;
sed -e 's/Bio Bio/Bío Bío/g' tmp1 > tmp;
sed -e 's/El Volcan/El Volcán/g' tmp > tmp1;
sed -e 's/Estacion Central/Estación Central/g' tmp1 > tmp;

mv tmp PhoneBusStopsSequences_dtpm.csv;
rm tmp1;
