#! /usr/bin/tclsh

package require base64

set UpdateIp(host) {members.dyndns.org}
set UpdateIp(port) {8245}
set UpdateIp(result) {none}
set UpdateIp(content) [list]
set UpdateIp(timeout) {10000} ; # milliseconds
set UpdateIp(done) 0

#set UpdateIp(statefile) [file join / var tmp UpdateDynDNS_state.dat]
set UpdateIp(statefile) [file join [pwd] UpdateDynDNS_state.dat]
set UpdateIp(enable) {false} ; # don't do anything unless good-to-go.
set UpdateIp(prev_timestamp) [clock seconds] ; # default to now.
set UpdateIp(prev_result) {none}
set UpdateIp(prev_ip) {127.0.0.1}

set DetectIp(host) {checkip.dyndns.org}
set DetectIp(resource) {/}
set DetectIp(port) {80}
set DetectIp(myIp) {127.0.0.1}
set DetectIp(timeout) {10000}  ; # milliseconds
set DetectIp(done) 0


proc sendHttpGetRequest {chan host resource {auth {NONE}}} { 
   puts $chan "GET $resource HTTP/1.0"
   puts $chan "Host: $host"
   if { {NONE} != $auth } {
      puts $chan "Authorization: Basic $auth"
   }
   puts $chan "User-agent: Self - Brad's DynDNS Update Client - v0.5 (14 Apr 2011)"
   puts $chan "Connection: close"
   puts $chan ""
}

proc loadState {} {
   global UpdateIp
   if { [catch {open $UpdateIp(statefile) "r"} f] } {
      puts stdout "failed to open state file for read"
   } else {
      if { ![eof $f] && ![catch {gets $f line}] } { set UpdateIp(enable) "$line" }
      if { ![eof $f] && ![catch {gets $f line}] } { set UpdateIp(prev_timestamp) "$line" } 
      if { ![eof $f] && ![catch {gets $f line}] } { set UpdateIp(prev_result) "$line" } 
      if { ![eof $f] && ![catch {gets $f line}] } { set UpdateIp(prev_ip) "$line" } 
      close $f
   }
}

proc saveState {} {
   global UpdateIp
   global DetectIp
   if { [catch {open $UpdateIp(statefile) "w"} f] } {
      puts stdout "failed to open state file for write"
   } else {
      set timestamp [clock format [clock seconds] -format {%d-%b-%Y %H:%M:%S}]
      puts $f "enabled"
      puts $f "$timestamp"
      puts $f "$UpdateIp(result)"
      puts $f "$DetectIp(myIp)"
      close $f
   }
}


proc receiveDetectIpReply {chan} {
   global DetectIp
   if { [eof $chan] || [catch {gets $chan reply}] } {
      close $chan
      # set DetectIp(done) 1
   } else {
      # puts stdout "$reply" 
      set matches [regexp -nocase -expanded  {([\d]+[\.][\d]+[\.][\d]+[\.][\d]+)}  $reply where addr]
      if { 0 < $matches } { 
         set DetectIp(myIp)  "$addr" 
         set DetectIp(done) 1
      }
   }
}

proc detectIp {} {
   global DetectIp
   set sock [socket -async $DetectIp(host) $DetectIp(port)]
   fconfigure $sock -blocking 0 -buffering line
   fileevent $sock readable [list receiveDetectIpReply $sock]
   fileevent $sock writable [list sendHttpGetRequest $sock $DetectIp(host) $DetectIp(resource)]

   after $DetectIp(timeout) set DetectIp(done) 1
   vwait DetectIp(done)
   return "$DetectIp(myIp)"
}

proc receiveUpdateIpReply {chan} {
   global UpdateIp
   if { [eof $chan] || [catch {gets $chan reply}] } {
      close $chan
      set UpdateIp(done) 1
   } else {
      lappend UpdateIp(content) $reply
      # puts stdout "$reply"
   }
}

proc updateIp {myhostname myipaddr auth} {
   global UpdateIp
   set resource "/nic/update?hostname=$myhostname&myip=$myipaddr"
   set sock [socket -async $UpdateIp(host) $UpdateIp(port)]
   fconfigure $sock -blocking 0 -buffering line
   fileevent $sock readable [list receiveUpdateIpReply $sock]
   fileevent $sock writable [list sendHttpGetRequest $sock $UpdateIp(host) $resource $auth]

   after $UpdateIp(timeout) set UpdateIp(done) 1
   vwait UpdateIp(done)

   return [getUpdateIpResult]
}

proc getUpdateIpResult {} {
   global UpdateIp
   set UpdateIp(result) {fail}
   if { 0 < [llength $UpdateIp(content)] } {
      set content [lindex $UpdateIp(content) end]
      set match [regexp -nocase -expanded {^([\w]+) [\s]+ ([\d]+[\.][\d]+[\.][\d]+[\.][\d]+)} $content where result ip]
      if { 0 < $match } {
         set UpdateIp(new_ip) $ip
         set UpdateIp(result) $result
      }
   }
   return "$UpdateIp(result)"
}


proc updateDynDNS {username password myHostName} {
   global UpdateIp
   set myAuth [base64::encode "$username:$password"]

   loadState

   if { {enabled} == $UpdateIp(enable) && {good} == $UpdateIp(prev_result) } {
      set myIp [detectIp]
      set prevIp "$UpdateIp(prev_ip)"

      puts stdout "current  ip addr: $myIp"
      puts stdout "previous ip addr: $prevIp"

      if { $myIp != $prevIp && $myIp != {127.0.0.1} } {
         updateIp $myHostName $myIp $myAuth
         saveState
         puts stdout "$UpdateIp(result)"
      }
   }
}


puts stdout "----------------------------------------------------------------------------"
set timestamp [clock format [clock seconds] -format {%d-%b-%Y %H:%M:%S}]
puts stdout "$timestamp"

set username {test}
set password {test}
set hostname {test.dyndns.org}
updateDynDNS $username $password $hostname


