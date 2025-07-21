// api-gateway/src/types/express.d.ts
import { AuthTokenPayload } from './index';

declare global {
  namespace Express {
    interface Request {
      user?: {
        userId: number;
        email: string;
      };
    }
  }
}
