#! /usr/bin/tclsh

package require http
package require base64

set Version {v0.9.5 (10-Nov-2014)}


# this stuff should come from a config file
set Config(detect_url) {http://checkip.dyndns.org:8245}
set Config(update_url) {http://members.dyndns.org:8245/nic/update/}
set Config(timeout) 10000
set Config(username) {test}
set Config(password) {test}
set Config(hostname) {test.dyndns.org}
#set Config(statefile) [file join / var run UpdateDynDNS_state.dat]
set Config(statefile) [file join [pwd] UpdateDynDNS_state.dat]

# this stuff comes from the state file
set PrevState(timestamp) [clock seconds] ; # default to now.
set PrevState(myIp) 127.0.0.1
set PrevState(status) none


proc loadState {path} {
    global PrevState
    if {[catch {open $path "r"} f]} {
        puts stdout "failed to open state file for read"
    } else {
        if { ![eof $f] && ![catch {gets $f line}] } {
            set PrevState(timestamp) "$line"
        }
        if { ![eof $f] && ![catch {gets $f line}] } {
            set result [split $line]
            set PrevState(status) [lindex $result 0]
            set PrevState(myIp) [lindex $result 1]
        }
        close $f
    }
}

proc saveState {path result} {
    if { [catch {open $path "w"} f] } {
        puts stdout "failed to open state file for write"
    } else {
        set timestamp [clock format [clock seconds] -format {%d-%b-%Y %H:%M:%S}]
        puts $f "$timestamp"
        puts $f "$result"
        close $f
    }
}


proc detectIp {url timeout} {
    set myIp {} ; # what this fctn returns
    set token [::http::geturl $url -timeout $timeout]
    set status [string tolower [::http::status $token]]
    set body [::http::data $token]
    http::cleanup $token

    if {[string match {ok} $status]} {
        set expression {([\d]+[\.][\d]+[\.][\d]+[\.][\d]+)}
        set count [regexp -nocase -expanded $expression $body all addr]
        if {$count > 0} {
            set myIp $addr
        } else {
            set myIp "could not find an IP address in the http response"
        }
    }

   return -code $status $myIp
}



proc updateIp {url timeout user passwd hostname myip} {
    global Version

    set useragent "Self - Brad's DynDNS Update Client (Tcl version) - $Version"
    ::http::config -useragent $useragent

    set hdrs [list Authorization "Basic [base64::encode $user:$passwd]"] 
    set query [::http::formatQuery hostname $hostname myip $myip]
    set token [::http::geturl "$url?$query" -headers $hdrs -timeout $timeout]
    set status [string tolower [::http::status $token]]
    set body [::http::data $token]
    # for debugging
    #upvar #0 $token state
    #puts stderr "requested url: '$state(url)'"
    http::cleanup $token

    #puts stderr "$status"
    #puts stderr "$body"

   return -code $status $body
}



proc updateDynDNS {} {
    global Config
    global PrevState

    puts stdout "----------------------------------------------------------------------------"
    set timestamp [clock format [clock seconds] -format {%d-%b-%Y %H:%M:%S}]
    puts stdout "$timestamp"

    loadState $Config(statefile)

    set prevIp "$PrevState(myIp)"
    set myIp {127.0.0.1}

    if {[catch {detectIp $Config(detect_url) $Config(timeout)} result]} {
        puts stderr "FAIL: detectIp $result"
    } else {
        set myIp "$result"
    }

    puts stdout "previous ip addr: $prevIp"
    puts stdout "current  ip addr: $myIp"

    set good_status [string match -nocase {good} $PrevState(status)]
    if {$good_status &&  $myIp != $prevIp && $myIp != {127.0.0.1} } {
        if {[catch {updateIp $Config(update_url) $Config(timeout) $Config(username) $Config(password) $Config(hostname) $myIp} result]} {
            puts stderr "FAIL: updateIp $result"
        } else {
            puts stdout "$result"
            saveState $Config(statefile) $result
        }
    }
}


# this could come from command line args.
#set configfile [file join [pwd] UpdateDynDNS_config.dat]

updateDynDNS

