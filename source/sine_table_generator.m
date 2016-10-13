clear; clc; close all;

% Parameters --------------------------------------------------------------
size = 2^12;
%--------------------------------------------------------------------------

% Generate graph
x = 1:size;
table = sin(2*pi*(x-0.5)/size);

% Print in rows of eight
for x = 1:(size - 1)
    fprintf('% f, ', table(x));
    if (mod(x,8) == 0)
        fprintf('\n');
    end
end
fprintf('% f\n', table(size));
