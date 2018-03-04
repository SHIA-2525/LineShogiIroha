-- GET POST AND TRANS UNIX DOMAIN SOCKET

-- character table string
local b='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
-- base64 encoding
function base64enc(data)
    return ((data:gsub('.', function(x) 
        local r,b='',x:byte()
        for i=8,1,-1 do r=r..(b%2^i-b%2^(i-1)>0 and '1' or '0') end
        return r;
    end)..'0000'):gsub('%d%d%d?%d?%d?%d?', function(x)
        if (#x < 6) then return '' end
        local c=0
        for i=1,6 do c=c+(x:sub(i,i)=='1' and 2^(6-i) or 0) end
        return b:sub(c+1,c+1)
    end)..({ '', '==', '=' })[#data%3+1])
end

local data = ngx.req.get_body_data()
if data then

  --check sig
  -- !この値は非公開! --
  local cs = "Channel Secret"

  local sig = ngx.req.get_headers()["X-Line-Signature"]
  if not sig then
    ngx.exit(ngx.HTTP_NOT_ALLOWED)
  end
  
  local hmac = require 'resty.hmac'

  local hmac_sha256 = hmac:new(cs, hmac.ALGOS.SHA256)
  if not hmac_sha256 then
    ngx.log(ngx.ERR, "failed to create the hmac_sha256 object")
    ngx.exit(ngx.HTTP_NOT_ALLOWED)
  end
  local ok = hmac_sha256:update(data)
  if not ok then
    ngx.log(ngx.ERR, "failed to add data")
    ngx.exit(ngx.HTTP_NOT_ALLOWED)
  end
  local mac = hmac_sha256:final()
  
  if not (sig == base64enc(mac)) then
    ngx.log(ngx.ERR, "sig:", sig)
    ngx.log(ngx.ERR, "mac:", base64enc(mac))
    ngx.exit(ngx.HTTP_NOT_ALLOWED)
  end

  -- send socket
  local sock = ngx.socket.tcp()
  ok, err = sock:connect("unix:/var/run/XXX.sock")
  if err then
    ngx.log(ngx.ERR, "connect: ", ok, " ", err)
    return
  end

  local bytes
  bytes, err = sock:send(data)
  if err then
    ngx.log(ngx.ERR, "send: ", bytes, " ", err)
  end

  ok, err = sock:close()
  if err then
    ngx.log(ngx.ERR, "close: ", ok, " ", err)
  end
  
  --ngx.log(ngx.ERR, data)
  --return
end

-- if overflows from the buffer, discards it.

-- body may get buffered in a temp file:
--local file = ngx.req.get_body_file()
--if file then
--  ngx.log(ngx.ERR, "why? set buffer=max? body is in file ", file)
--else
--  ngx.say("no body found")
--  ngx.log(ngx.ERR, "no body found")
--end

--EOF--

