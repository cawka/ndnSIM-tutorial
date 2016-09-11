#!/usr/bin/env Rscript
# Copyright (c) 2012-2013  Alexander Afanasyev <alexander.afanasyev@ucla.edu>


library (ggplot2)
data = read.table ("results/rate-trace.txt", header=T)
g = ggplot(data, aes(x=Time, y=Kilobytes, color=Type)) + geom_line () + facet_wrap(~ FaceDescr)
pdf ('1.pdf', width = 8, height = 4)
print (g)
dev.off ()

library (ggplot2)
data = read.table ("results/app-delays-trace.txt", header=T)
g = ggplot(data, aes(x=Time, y = DelayS, color=Type)) + geom_point ()
pdf ('2.pdf', width = 5, height = 3)
print (g)
dev.off ()

library (ggplot2)
data = read.table ("results/cs-trace.txt", header=T)
g = ggplot(data, aes(x=Time, y = Packets, color=Type)) + geom_point () + facet_wrap (~ Node)
pdf ('3.pdf', width = 8, height = 4)
print (g)
dev.off ()
