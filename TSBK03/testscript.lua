p = game:getPlayer()

print("Position", p:getPosition())
print("Facing", p:getFacing())

e = game:getCurrentTarget()

if e ~= nil then
	print("Enemy Position", e:getPosition())
end