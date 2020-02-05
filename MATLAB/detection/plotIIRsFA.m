function [] = plotIIRsFA( ipath )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    a = dir(ipath);
%     hold off
    n = numel(a);
    hold off
    kolory = {'k-', 'k--', '', '', 'k:'}
    for idx = 3:n
        if ~a(idx).isdir
            filename = [ipath '\' a(idx).name];
            fprintf('Loading %s\n',filename);
            f = loadiir(filename);
            sfreq = filename(end-6:end-4);
            freq = sscanf(sfreq,'%d');
            
            [h,w] = freqz(f.b,f.a,2^16,freq);
            d = sscanf(f.name(3:end),'%f-%f');
            loglog(w,abs(h),kolory{freq / 100})
            hold on
            if numel(d) > 1
                text((4*d(1)+d(2))/5, 0.9, f.name(3:end), 'Rotation', -90)
            else
                text(d*1.2, 0.9, f.name(3:end), 'Rotation', -90)
        end
    end
    axis([0.7 30 0.1 1])
    xlabel('Frequency [Hz]')
end

