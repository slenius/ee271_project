clear all
close all

A = imread('assignment1/out.ppm');
B = imread('vect/vec_271_01_sv_ref.ppm');

C = A - B;

%imshow(C);

D = C * (255 / max(max(max(C))));

imshow(D);
