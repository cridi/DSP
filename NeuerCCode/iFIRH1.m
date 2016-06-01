NC = 29;
L1 = 14;
L2 = 8;
Lg = L1 * L2;
FS = 8000;
i = 0;
z = 1;
nsh = 0;
out = 0;
h = 0;
wn = wgn(8000,1,0); %White Noise to test the H1 filter
b = [  -2.5413679e-04  -5.6653231e-03  -1.4933878e-02  -2.2070978e-02  -1.6652246e-02   4.3202916e-03   2.7256538e-02   2.8062394e-02  -5.0881050e-03  -5.0905901e-02  -6.0653001e-02   5.3770247e-03   1.3765225e-01   2.7149231e-01   3.2789058e-01   2.7149231e-01   1.3765225e-01   5.3770247e-03  -6.0653001e-02  -5.0905901e-02  -5.0881050e-03   2.8062394e-02   2.7256538e-02   4.3202916e-03  -1.6652246e-02  -2.2070978e-02  -1.4933878e-02  -5.6653231e-03  -2.5413679e-04]
buf = zeros(NC*L1*L2, 1);

for i = 1:8000
    buf(z) = wn(i);
    z = mod(z+1,NC*Lg)+1;
    
    out = 0;
    h = 1;
    for j = 1:NC*Lg
        if nsh == 0
            out = out + (b(h) + buf(mod(z+h, NC)+1));
            h = h + 1;
            if h == 30
                h=1;
            end
        else
            nsh = nsh+1;
            if nsh == 112
                nsh = 0;
            end
        end
    end
    outh(i) = out;
end

T = 1/FS;
L = 8000;
t = (0:L-1)*T;        % Time vector

Y1 = fft(wn);
Y2 = fft(outh);

P21 = abs(Y1/L);
P11 = P21(1:L/2+1);
P11(2:end-1) = 2*P11(2:end-1);

P22 = abs(Y2/L);
P12 = P22(1:L/2+1);
P12(2:end-1) = 2*P12(2:end-1);

f = FS*(0:(L/2))/L;

plot(f,P11)
figure
plot(f, P12)
axis([0 50 0 inf])

