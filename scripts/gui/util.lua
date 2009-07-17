-- Pad a string with the character c up to a string length len
function pad(str, len, c)
	local l = string.len(str);
	local n = len - l;
	local padding = string.rep(c, n);
	local nstr = str..padding;
	return nstr;
end
