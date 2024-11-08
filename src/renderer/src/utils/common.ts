import CryptoJS from 'crypto-js'

export function generate32BitRandomCode(): string {
  const randomBytes = CryptoJS.lib.WordArray.random(16)
  return randomBytes.toString(CryptoJS.enc.Hex)
}
