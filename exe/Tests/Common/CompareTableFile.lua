--The functions compares two csv files that store numeric data

local function compareData(fn1, fn2, fnt)
	local f1 = io.open(fn1, 'rt')
	if f1 == nil then
		return 'unable to open file '..fn1
	end
	local f2 = io.open(fn2, 'rt')
	if f2 == nil then
		return 'unable to open file '..fn2
	end
	local ft = io.open(fnt, 'rt')
	if ft == nil then
		return 'unable to open file '..fnt
	end	
	
	local fieldIndex = 0
	while true do
		fieldIndex = fieldIndex + 1
		
		local linet = ft:read()
		local tolerance = linet and tonumber(string.match(linet, "[eE%d-\.]+"))
		if tolerance == nil then
			return nil
		end
		
		local line1 = f1:read()
		local line2 = f2:read()
		
		if (line1 ~= nil) ~= (line2 ~= nil) then
			if line1 == nil then
				return 'missed field '..fieldIndex..' in 1'
			else
				return 'missed field '..fieldIndex..' in 2'
			end
		elseif (line1 == nil) and (line2 == nil) then
			f1:close()
			f2:close()
			ft:close()
			return nil
		end
		
		local tolerance = linet and tonumber(string.match(linet, "[eE%d-\.]+"))
		if tolerance == nil then
			return 'no tolerance specified for field '..fieldIndex
		end
		
		local elementIndex = 0
		local fieldElementPos1 = 1
		local fieldElementPos2 = 1
		while true do
			elementIndex = elementIndex + 1
			local start1, end1 = string.find(line1, "[eE%d-\.]+", fieldElementPos1)
			local start2, end2 = string.find(line2, "[eE%d-\.]+", fieldElementPos2)
			if (start1 ~= nil) ~= (start2 ~= nil) then
				if start1 == nil then
					return 'missed element '..elementIndex..' of field '..fieldIndex..' in 1'
				else
					return 'missed element '..elementIndex..' of field '..fieldIndex..' in 2'
				end
			elseif (start1 == nil) and (start2 == nil) then
				break
			end
			local value1 = tonumber(string.sub(line1, start1, end1))
			local value2 = tonumber(string.sub(line2, start2, end2))
			if (value1 ~= nil) ~= (value2 ~= nil) then
				if value1 == nil then
					return 'missed element '..elementIndex..' of field '..fieldIndex..' in 1'
				else
					return 'missed element '..elementIndex..' of field '..fieldIndex..' in 2'
				end
			elseif (value1 == nil) and (value2 == nil) then
				break
			end				
			local delta = math.abs(value1 - value2)
			if delta > tolerance then
				return 'element '..elementIndex..' of field '..fieldIndex..' not matched: '..tostring(value1)..' - '..tostring(value2)..' = '..tostring(delta)..' > '..tostring(tolerance)..'\n'
			end
			fieldElementPos1 = end1 + 1
			fieldElementPos2 = end2 + 1
		end
	end
	assert(true)
end

local fn1 = arg[1]
local fn2 = arg[2]
local fnt = arg[3]

local result = compareData(fn1, fn2, fnt)
if result ~= nil then
	print('*** Comparing')
	print(fn1)
	print('			and')
	print(fn2)
	print('			with tolerance')
	print(fnt)
	print('\nError found: '..result)
else
	print('OK')
end