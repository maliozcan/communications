#!/usr/bin/gnuplot -p

set datafile separator ','
set format y "10^{%L}
set logscale y
set grid
plot 'ber.txt' with lines title "BER", 'ber2.txt' with lines title "BER2"


#   angles            arrow             autoscale         bars
#   bmargin           border            boxdepth          boxwidth
#   cbdata            cbdtics           cblabel           cbmtics
#   cbrange           cbtics            clabel            clip
#   cntrlabel         cntrparam         color             colorbox
#   colorsequence     contour           dashtype          data
#   datafile          date_specifiers   decimalsign       dgrid3d
#   dummy             encoding          errorbars         fit
#   fontpath          format            function          grid
#   hidden3d          history           historysize       isosamples
#   isosurface        jitter            key               label
#   linetype          link              lmargin           loadpath
#   locale            log               logscale          macros
#   mapping           margin            margins           micro
#   minussign         missing           monochrome        mouse
#   mttics            multiplot         mx2tics           mxtics
#   my2tics           mytics            mztics            nonlinear
#   object            offsets           origin            output
#   overflow          palette           parametric        paxis
#   pixmap            pm3d              pointintervalbox  pointsize
#   polar             print             psdir             range
#   raxis             rgbmax            rlabel            rmargin
#   rrange            rtics             samples           size
#   spiderplot        style             surface           table
#   term              terminal          termoption        theta
#   tics              ticscale          ticslevel         time_specifiers
#   timefmt           timestamp         title             tmargin
#   trange            ttics             urange            vgrid
#   view              vrange            vxrange           vyrange
#   vzrange           walls             x2data            x2dtics
#   x2label           x2mtics           x2range           x2tics
#   x2zeroaxis        xdata             xdtics            xlabel
#   xmtics            xrange            xtics             xyplane
#   xzeroaxis         y2data            y2dtics           y2label
#   y2mtics           y2range           y2tics            y2zeroaxis
#   ydata             ydtics            ylabel            ymtics
#   yrange            ytics             yzeroaxis         zdata
#   zdtics            zero              zeroaxis          zlabel
#   zmtics            zrange            ztics             zzeroaxis
