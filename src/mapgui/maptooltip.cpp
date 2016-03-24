/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "maplayer.h"
#include "maptooltip.h"
#include "common/maptypes.h"
#include "table/formatter.h"

using namespace maptypes;

MapTooltip::MapTooltip(QObject *parent)
  : QObject(parent)
{

}

QString MapTooltip::buildTooltip(maptypes::MapSearchResult& mapSearchResult, const MapLayer *mapLayer)
{
  QString text;
  for(const MapAirport *ap : mapSearchResult.airports)
  {
    if(!text.isEmpty())
      text += "<hr/>";

    text += "<b>" + ap->name + " (" + ap->ident + ")</b>";
    text += "<br/>Longest Runway: " + QLocale().toString(ap->longestRunwayLength) + " ft";
    text += "<br/>Altitude: " + QLocale().toString(ap->altitude) + " ft";
    text += "<br/>Magvar: " + formatter::formatDoubleUnit(ap->magvar, QString(), 1) + " °";

    if(ap->towerFrequency > 0)
      text += "<br/>Tower: " + formatter::formatDoubleUnit(ap->towerFrequency / 1000., QString(), 2);
    if(ap->atisFrequency > 0)
      text += "<br/>ATIS: " + formatter::formatDoubleUnit(ap->atisFrequency / 1000., QString(), 2);
    if(ap->awosFrequency > 0)
      text += "<br/>AWOS: " + formatter::formatDoubleUnit(ap->awosFrequency / 1000., QString(), 2);
    if(ap->asosFrequency > 0)
      text += "<br/>ASOS: " + formatter::formatDoubleUnit(ap->asosFrequency / 1000., QString(), 2);
    if(ap->unicomFrequency > 0)
      text += "<br/>Unicom: " + formatter::formatDoubleUnit(ap->unicomFrequency / 1000., QString(), 2);

    if(ap->addon())
      text += "<br/>Add-on Airport";
    if(ap->flags.testFlag(AP_MIL))
      text += "<br/>Military Airport";
    if(ap->scenery())
      text += "<br/>Has some Scenery Elements";

    if(ap->hard())
      text += "<br/>Has Hard Runways";
    if(ap->soft())
      text += "<br/>Has Soft Runways";
    if(ap->water())
      text += "<br/>Has Water Runways";
    if(ap->helipad())
      text += "<br/>Has Helipads";
    if(ap->flags.testFlag(AP_LIGHT))
      text += "<br/>Has Lighted Runways";
    if(ap->flags.testFlag(AP_AVGAS))
      text += "<br/>Has Avgas";
    if(ap->flags.testFlag(AP_JETFUEL))
      text += "<br/>Has Jetfuel";

    if(ap->flags.testFlag(AP_ILS))
      text += "<br/>Has ILS";
    if(ap->flags.testFlag(AP_APPR))
      text += "<br/>Has Approaches";
  }

  for(const MapVor *vor : mapSearchResult.vors)
  {
    if(!text.isEmpty())
      text += "<hr/>";
    QString type;
    if(vor->dmeOnly)
      type = "DME";
    else if(vor->hasDme)
      type = "VORDME";
    else
      type = "VOR";

    text += "<b>" + type + ": " + vor->name + " (" + vor->ident + ")</b>";
    text += "<br/>Type: " + maptypes::navTypeName(vor->type);
    text += "<br/>Region: " + vor->region;
    text += "<br/>Freq: " + formatter::formatDoubleUnit(vor->frequency / 1000., QString(), 2) + " MHz";
    if(!vor->dmeOnly)
      text += "<br/>Magvar: " + formatter::formatDoubleUnit(vor->magvar, QString(), 1) + " °";
    text += "<br/>Range: " + QString::number(vor->range) + " nm";
  }

  for(const MapNdb *ndb : mapSearchResult.ndbs)
  {
    if(!text.isEmpty())
      text += "<hr/>";
    text += "<b>NDB: " + ndb->name + " (" + ndb->ident + ")</b>";
    text += "<br/>Type: " + maptypes::navTypeName(ndb->type);
    text += "<br/>Region: " + ndb->region;
    text += "<br/>Freq: " + formatter::formatDoubleUnit(ndb->frequency / 100., QString(), 2) + " kHz";
    text += "<br/>Magvar: " + formatter::formatDoubleUnit(ndb->magvar, QString(), 1) + " °";
    text += "<br/>Range: " + QString::number(ndb->range) + " nm";
  }

  for(const MapWaypoint *wp : mapSearchResult.waypoints)
  {
    if(!text.isEmpty())
      text += "<hr/>";
    text += "<b>Waypoint: " + wp->ident + "</b>";
    text += "<br/>Type: " + maptypes::navTypeName(wp->type);
    text += "<br/>Region: " + wp->region;
    text += "<br/>Magvar: " + formatter::formatDoubleUnit(wp->magvar, QString(), 1) + " °";
  }

  for(const MapMarker *m : mapSearchResult.markers)
  {
    if(!text.isEmpty())
      text += "<hr/>";
    text += "<b>Marker: " + m->type + "</b>";
  }

  if(mapLayer != nullptr && mapLayer->isAirportDiagram())
  {
    for(const MapAirport *ap : mapSearchResult.towers)
    {
      if(!text.isEmpty())
        text += "<hr/>";

      if(ap->towerFrequency > 0)
        text += "Tower: " + formatter::formatDoubleUnit(ap->towerFrequency / 1000., QString(), 2);
      else
        text += "Tower";
    }
    for(const MapParking *p : mapSearchResult.parkings)
    {
      if(!text.isEmpty())
        text += "<hr/>";

      text += maptypes::parkingName(p->name) + " " + QString::number(p->number) +
              "<br/>" + maptypes::parkingTypeName(p->type) +
              "<br/>" + QString::number(p->radius * 2) + " ft" +
              (p->jetway ? "<br/>Has Jetway" : "");

    }
    for(const MapHelipad *p : mapSearchResult.helipads)
    {
      if(!text.isEmpty())
        text += "<hr/>";

      text += QString("Helipad:") +
              "<br/>Surface: " + maptypes::surfaceName(p->surface) +
              "<br/>Type: " + p->type +
              "<br/>" + QString::number(p->width) + " ft" +
              (p->closed ? "<br/>Is Closed" : "");

    }
  }
  return text;
}
