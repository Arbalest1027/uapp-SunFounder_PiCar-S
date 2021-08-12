% A = importdata('distant.txt', ' ', 1);
% B = readtable('distant.txt');
% B.time
% plot(B.time,B.distance)
clc;
clear all;
subplot(2,1,1)
B = readtable('distant0.txt');
plot(B.time,B.distance)
yline(40,'-','Slow');
yline(30,'-','Slower');
grid on
xlabel('time');
ylabel('dist');
subplot(2,1,2)
B = readtable('speed0.txt');
plot(B.time,B.speed)
grid on
xlabel('time');
ylabel('speed');
% subplot(2,2,3)
% B = readtable('distant2.txt');
% plot(B.time,B.distance)
% grid on
% xlabel('time');
% ylabel('dist');
% subplot(2,2,4)
% B = readtable('distant3.txt');
% plot(B.time,B.distance)
% grid on
% xlabel('time');
% ylabel('dist');
print('distantvsspeed','-dpng');