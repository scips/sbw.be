class EquationOfTime
  constructor: (date) ->
    @init(date)

  W: () ->
    360/365.24

  init: (date) ->
    now = new Date()
    now.setHours(0)
    now.setMinutes(0)
    now.setSeconds(0)
    now.setDate(1)
    now.setMonth(0)
    @D = Math.floor( (date.getTime() - now.getTime())/86400000 )
    
  A: () ->
    @W()*(@D+10)

  B: () ->
    @A() + 360/Math.PI * 0.0167 * Math.sin(@W()*(@D-2))

  C: () ->
    ( @A()-Math.atan( Math.tan(@B()) / Math.cos(23.44) ) )/180

  EOT: () ->
    C = @C()
    720 * ( C - Math.round(C))

  Declination: () ->
    0-(Math.asin(Math.sin(23.44)*Math.cos(@B()) ))

if(process.argv[2])
  longitude = parseFloat(process.argv[2])
else
  longitude = 0

if(process.argv[3])
  timenet = parseFloat(process.argv[3])
else
  timenet = 0

d = new Date()
eot = new EquationOfTime(d)

console.log "W: #{eot.W()}"
console.log "A: #{eot.A()}"
console.log "B: #{eot.B()}"
console.log "C: #{eot.C()}"
eotValue = eot.EOT()
console.log "EOT: #{eotValue}"
console.log "EOT+longitude: #{ eotValue + longitude - timenet}"
console.log "Declination: #{eot.Declination()}"
