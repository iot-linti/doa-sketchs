factor = 50;
archivos=dir('*.CSV');
figure
for i=1:length(archivos)-1
  # carga de datos y recorte
  datosInicial(:,:,i) = csvread(archivos(i).name);
  datos(:,:,i) = datosInicial(3:size(datosInicial)(1),4:6,i);

  # separacion de datos del csv
  x(:,i) = datos(:,1,i);
  v1(:,i) = datos(:,2,i);
  v2(:,i) = datos(:,3,i);
  shuntvolt(:,i) = v1(:,i) - v2(:,i);

  # filtrado (promedio movil)
  coef(:,i) = ones(1, factor)/factor;
  promshunt(:,i) = filter(coef(:,i), 1, shuntvolt(:,i));
  
  # grafico
  subplot(2,2,i)
  plot(x(:,i), shuntvolt(:,i), 'b', x(:,i), promshunt(:,i), 'r')
  title(archivos(i).name)
end

# codigo fuera del bucle for porque udp.CSV tiene distinto tamaño que los otros CSVs
datosInicial4(:,:) = csvread(archivos(4).name);
datos4(:,:) = datosInicial4(3:size(datosInicial4)(1),4:6);

x4 = datos4(:,1);
v14 = datos4(:,2);
v24 = datos4(:,3);
shuntvolt4 = v14 - v24;

coef4 = ones(1, factor)/factor;
promshunt4 = filter(coef4, 1, shuntvolt4);

subplot(2,2,4)
plot(x4, shuntvolt4, 'b', x4, promshunt4, 'r')
title(archivos(4).name)