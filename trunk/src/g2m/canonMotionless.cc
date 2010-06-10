#include <string>
#include "canonMotionless.hh"
#include "uio.hh"

canonMotionless::canonMotionless(std::string canonL, machineStatus prevStatus):canonLine(canonL, prevStatus) {
  match = true;
  handled = true;
  status.setEndPose(status.getStartPose());

  //match canonical commands. the string MUST be the complete command name
  if (clMatch("COMMENT")) {
    //do nothing
  } else if (clMatch("MESSAGE")) {
    uio::infoMsg("Message: " + canonTokens[3]);
  //} else if (clMatch("SPINDLE")) {
  //} else if (clMatch("MIST")) {
  //} else if (clMatch("ENABLE")) {
  //} else if (clMatch("FLOOD")) {
  } else if (clMatch("DWELL")) {
  //} else if (clMatch("FEEDRATE")) {
  } else if (clMatch("SET_FEED_RATE")) {
    handled = false;
  } else if (clMatch("SET_FEED_REFERENCE")) {
    handled = false;
  } else if (clMatch("SELECT_TOOL")) {
    handled = false;
  } else if (clMatch("CHANGE_TOOL")) {
    handled = false;
  } else if (clMatch("USE_TOOL_LENGTH_OFFSET")) {
    handled = false;
  } else if (clMatch("PROGRAM_STOP")) {
  } else if (clMatch("SET_ORIGIN_OFFSETS")) {
    
    if (std::string::npos != canonL.find("SET_ORIGIN_OFFSETS(0.0000,")) {
      uio::infoMsg("Warning, input has reduced precision - expected more zeros: \n" + canonL );
    }
    handled = false; //because I still don't know what to do if we have the correct data...
  } else if (clMatch("USE_LENGTH_UNITS")) {
    handled = false;
  } else if (clMatch("SET_MOTION_CONTROL_MODE")) {
    handled = false;
  } else if (clMatch("SET_XY_ROTATION")) {
    handled = false;
  } else if (clMatch("SET_FEED_REFERENCE")) {
    handled = false;
  } else if (clMatch("SET_NAIVECAM_TOLERANCE")) {
    handled = false;
  } else if (clMatch("PROGRAM_END")) {
  } else match = false;

  if ( !match || !handled ) {
    std::string m;
    if (!handled) 
      m = "Warning, unhandled";
    else m = "Error, unknown";
    uio::infoMsg(m + " canonical command: " + canonL);
  }
}
/*
SET_ORIGIN_OFFSETS(0.0000,  //this is a common canon statement. we are going to hijack it to produce a warning, because
        //the data we're getting was produced with a format of %.4f or so
        infoMsg(QString("Warning, input has reduced precision, expected more zeros: <br>") + canon_line );
*/

/*
bool clMatch(string m) {
  return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
}
*/
