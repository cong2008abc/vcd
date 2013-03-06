import timeit
import count

count.init(".")
t = timeit.Timer("read_db('/mnt/share/om-test/20130214_020215.omf')", "from count import read_db")
print t.timeit(1)
