local function makeIndex(index, format)	
	if type(index) == 'string' then
		if format ~= nil and format.brackets then
			return '[\''..index..'\']'
		else
			return index
		end
	else
		return tostring(index)
	end
end

local makeTable = nil

local function makeValue(value, level, format)
	if type(value) == 'table' then	
		return makeTable(value, level + 1, format)
	else
		if type(value) == 'string' then
			return '\''..value..'\''
		else
			if format ~= nil and format.format ~= nil then
				return string.format(format.format, value)
			else
				return tostring(value)
			end		
		end
	end
end

local function getSubFormat(format, index)
	if format ~= nil then
		if format._sub ~= nil then
			if format._sub[index] ~= nil then
				return format._sub[index]
			end
		end
		if format._all then
			return format._all
		elseif format._inherit then
			return format
		end
	end
end

local function find(tbl, val)
	for index, value in pairs(tbl) do
		if value == val then
			return true
		end
	end
	return false
end

local function makeIndexes(tbl, format)
	local result = {}
	if format.order ~= nil then
		for number, index in pairs(format.order) do
			table.insert(result, index)
		end
		for index, value in pairs(tbl) do
			if not find(result, index) then
				table.insert(result, index)
			end
		end
	else
		for index, value in pairs(tbl) do
			table.insert(result, index)
		end
	end
	return result
end

local function makeTableSingleLine(tbl, level, format, arrayFlag)
	local str = ''
	local first = true	
	local indexes = makeIndexes(tbl, format)
	local prevComment = nil
	for indexNum, index in pairs(indexes) do
		local value = tbl[index]
		if not first then
			str = str..', '
			if prevComment ~= nil then
				str = str..' /*'..prevComment..'*/'
			end			
		end
		first = false
		local subFormat = getSubFormat(format, index)
		if not arrayFlag then	
			str = str..makeIndex(index, format)..' = '..makeValue(value, level, subFormat)
		else
			str = str..makeValue(value, level, subFormat)
		end
		prevComment = subFormat and subFormat.comment
	end
	return str
end

local function fill(count, char)
	local result = ''
	for i = 1, count do
		result = result..char
	end
	return result
end

local function makeTableMultiLine(tbl, level, format, arrayFlag, commas)
	local str = ''
	local first = true
	local shift = fill(level, '\t')
	local indexes = makeIndexes(tbl, format)
	local prevComment = nil
	for indexNum, index in pairs(indexes) do
		local value = tbl[index]		
		if not first then
			if commas then
				str = str..','
			end
			if prevComment ~= nil then
				str = str..' --'..prevComment
			end
			str = str..'\n'
		end
		first = false
		local subFormat = getSubFormat(format, index)
		if not arrayFlag then	
			str = str..shift..makeIndex(index, format)..' = '..makeValue(value, level, subFormat)
		else
			str = str..shift..makeValue(value, level, subFormat)
		end
		prevComment = subFormat and subFormat.comment
	end
	str = str..'\n'..fill(level - 1, '\t')
	return str
end

local function isArray(tbl)
	local prevIndex = 0
	for index, val in pairs(tbl) do
		if type(index) ~= 'number' then
			return false
		elseif index - prevIndex ~= 1 then
			return false
		end
		prevIndex = index
	end
	return true
end

makeTable = function(tbl, level, format)
	if format ~= nil and format.singleLine then
		return '{'..makeTableSingleLine(tbl, level, format, isArray(tbl))..'}'
	else
		return '{\n'..makeTableMultiLine(tbl, level, format, isArray(tbl), true)..'}'
	end
end

function tableToString(tbl, format)
	if format ~= nil and format.singleLine then
		return makeTableSingleLine(tbl, 0, format, isArray(tbl))
	else
		return makeTableMultiLine(tbl, 0, format, isArray(tbl), false)
	end
end