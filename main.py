from string import digits, ascii_lowercase

nums = []
for i in "score":
    nums.append((digits+ascii_lowercase).index(i) + 55)
    
print(nums)